// augr/fm/dexie.cpp

#include <algorithm>
#include <cmath>

#include <augr/fm/dexie.h>
#include <augr/fm/dexie_common.h>

#include <augr/ui/ui_builder.h>

namespace augr::fm {

namespace {

constexpr float kTwoPi = 6.28318530717958647692f;

float CvToFreq(float cv) { return 261.6255653f * std::pow(2.f, cv); }

constexpr int kFeedbackBitdepth = 8;

// DX7 ampmodsenstab, ported directly from Dexed (Dx7Note.cpp). Values are
// 24-bit fixed point (1<<24 = unity); converted to float here.
constexpr float kAmpModSensTab[4] = {
    0.0f,
    4342338.0f / 16777216.0f,
    7171437.0f / 16777216.0f,
    16777216.0f / 16777216.0f,
};

// Raw pitchmodsenstab values [0..255], matching Dexed's fixed-point domain.
constexpr int kPitchModSensRaw[8] = {0, 10, 20, 33, 55, 92, 153, 255};

// dexie.cpp — new free functions in the anonymous namespace:
const uint8_t kExpScaleData[] = {
    0,  1,  2,  3,  4,  5,  6,   7,   8,   9,   11,  14,  16,  19,  23,  27, 33,
    39, 47, 56, 66, 80, 94, 110, 126, 142, 158, 174, 190, 206, 222, 238, 250};

int ScaleCurve(int group, int depth, int curve) {
    int scale;
    if (curve == 0 || curve == 3) {
        scale = (group * depth * 329) >> 12;
    } else {
        int n = sizeof(kExpScaleData);
        int raw_exp = kExpScaleData[std::min(group, n - 1)];
        scale = (raw_exp * depth * 329) >> 15;
    }
    if (curve < 2)
        scale = -scale;
    return scale;
}

int ScaleLevel(int midinote, int break_pt, int left_depth, int right_depth,
               int left_curve, int right_curve) {
    int offset = midinote - break_pt - 17;
    if (offset >= 0)
        return ScaleCurve((offset + 1) / 3, right_depth, right_curve);
    else
        return ScaleCurve(-(offset - 1) / 3, left_depth, left_curve);
}

int ScaleRate(int midinote, int sensitivity) {
    const int x = std::clamp(midinote / 3 - 7, 0, 31);
    return (sensitivity * x) >> 3;
}

const uint8_t kVelocityData[64] = {
    0,   70,  86,  97,  106, 114, 121, 126, 132, 138, 142, 148, 152,
    156, 160, 163, 166, 170, 173, 174, 178, 181, 184, 186, 189, 190,
    194, 196, 198, 200, 202, 205, 206, 209, 211, 214, 216, 218, 220,
    222, 224, 225, 227, 229, 230, 232, 233, 235, 237, 238, 240, 241,
    242, 243, 244, 246, 246, 248, 249, 250, 251, 252, 253, 254};

int ScaleVelocity(int velocity_0_127, int sensitivity_0_7) {
    const int clamped_vel = std::clamp(velocity_0_127, 0, 127);
    const int vel_value = kVelocityData[clamped_vel >> 1] - 239;
    const int scaled_vel = ((sensitivity_0_7 * vel_value + 7) >> 3) << 4;
    return scaled_vel;
}

// Faithful port of the AMD/AMS amplitude-modulation level reduction from
// Dx7Note::compute. Returns a [0,1] multiplier to apply to the envelope's
// output amplitude. lfo_val is bipolar [-1,1] (LfoModule's convention);
// amd_depth_norm and ams_depth_norm are both normalized [0,1]; delay_ramp
// is the note-on fade-in fraction [0,1]. The exponential shape (not a
// linear blend) is what gives this its percussive "gating" character
// rather than smooth tremolo — see Dexed's own "needs real tuning" comment
// on the 262144/0.07/12.2 constants; treat those as approximate.
float AmpModLevelMultiplier(float lfo_val, float amd_depth_norm,
                            float ams_depth_norm, float delay_ramp) {
    const float inv_lfo = (1.0f - lfo_val) * 0.5f;
    const float amod_1 = amd_depth_norm * delay_ramp * inv_lfo;
    const float sensamp = amod_1 * ams_depth_norm;

    if (sensamp <= 0.0f)
        return 1.0f;

    const float raw_sensamp = sensamp * 16777216.0f;
    const float pt = std::exp(raw_sensamp / 262144.0f * 0.07f + 12.2f);
    float ldiff_frac = (pt * 16.0f) / 268435456.0f;
    ldiff_frac = std::clamp(ldiff_frac, 0.0f, 1.0f);
    return 1.0f - ldiff_frac;
}

// DX7 detune is pitch-dependent, evaluated at the operator's own frequency
// (post-ratio). Ported from Dexed's osc_freq (dx7note.cc).
// Reference 8.175799 Hz = MIDI note 0 (C-1), Dexed's logfreq=0 anchor.
// detune_ is centered (-7..7) via SysexDetuneToParam.
float DetuneHz(float op_hz, float detune_centered) {
    if (detune_centered == 0.0f)
        return 0.0f;
    const float log_normalized = std::log2(op_hz / 8.175799f);
    const float detune_ratio =
        0.0209f * std::exp(-0.396f * log_normalized) / 7.0f;
    return op_hz * detune_ratio * detune_centered;
}

// DX7 coarse ratio encoding:
//   0      → 0.5
//   1..31  → 1..31
float CoarseToRatio(uint8_t coarse) {
    return coarse == 0 ? 0.5f : static_cast<float>(coarse);
}

// DX7 fine ratio: 0..99 → multiplicative fraction 0..~0.99
// The fine value represents (1 + fine/100) - 1 = fine/100 additive on top of
// coarse.
float FineToRatioFine(uint8_t fine) { return static_cast<float>(fine) / 100.f; }

} // namespace

void Dexie::OnCreate() {
    Module::OnCreate();
    label_ = "Dexie";
}

void Dexie::CreatePins() {
    cv_pitch_in_ = new VoltageInput(*this, "pitch");
    AddInput(*cv_pitch_in_);
    gate_in_ = new VoltageInput(*this, "gate");
    AddInput(*gate_in_);
    cv_velocity_in_ = new VoltageInput(*this, "velocity");
    AddInput(*cv_velocity_in_);

    cv_phase_in_ = new VoltageInput(*this, "phase");
    // cv_phase_in_ = new MixingAudioInput(*this, "phase");
    AddInput(*cv_phase_in_);

    cv_amp_mod_in_ = new VoltageInput(*this, "amp_mod");
    AddInput(*cv_amp_mod_in_);
    cv_pitch_mod_in_ = new VoltageInput(*this, "pitch_mod");
    AddInput(*cv_pitch_mod_in_);

    cv_pitch_env_in_ = new VoltageInput(*this, "pitch_env");
    AddInput(*cv_pitch_env_in_);

    audio_out_ = new AudioOutput(*this, "out", ChannelLayout::kMono);
    AddOutput(*audio_out_);
}

void Dexie::CreateControls() {
    UiBuilder ui(console_);

    // Oscillator section
    {
        auto _ = ui.HBox("Oscillator");
        coarse_param_ = CreateIntParameter("Coarse", ControlMeta::kDefault,
                                           &params_.coarse, 1, 0, 16, 1);
        fine_param_ = CreateIntParameter("Fine", ControlMeta::kDefault,
                                         &params_.fine, 0, 0, 99, 1);
        detune_param_ = CreateIntParameter("Detune", ControlMeta::kDefault,
                                           &params_.detune, 0, -7, 7, 1);
        level_param_ =
            CreateIntParameter("Level", ControlMeta::kDefault,
                               &params_.output_level, 99.f, 0.f, 99.f, 1.f);
        feedback_param_ = CreateIntParameter("Feedback", ControlMeta::kDefault,
                                             &params_.feedback, 0, 0, 7, 1);

        transpose_param_ =
            CreateIntParameter("Transpose", ControlMeta::kDefault,
                               &params_.transpose, 0, -24, 24, 1);
        fixed_freq_param_ = CreateBoolParameter(
            "Fixed Freq", ControlMeta::kDefault, &params_.fixed_freq, false);
        osc_key_sync_param_ = CreateBoolParameter(
            "Key Sync", ControlMeta::kDefault, &params_.osc_key_sync, false);

        ui.KnobInt("Coarse", coarse_param_);
        ui.KnobInt("Fine", fine_param_);
        ui.KnobInt("Detune", detune_param_);
        ui.KnobInt("Level", level_param_);
        ui.KnobInt("Feedback", feedback_param_);
        ui.KnobInt("Transpose", transpose_param_);
        ui.CheckButtonBool("Fixed Freq", fixed_freq_param_);
        ui.CheckButtonBool("Key Sync", osc_key_sync_param_);
    }

    // EG section — matches DX7 operator page ordering
    {
        auto _ = ui.HBox("EG Rates");
        r1_param_ = CreateIntParameter("R1", ControlMeta::kDefault,
                                       &params_.rates[0], 99, 0, 99, 1);
        r2_param_ = CreateIntParameter("R2", ControlMeta::kDefault,
                                       &params_.rates[1], 50, 0, 99, 1);
        r3_param_ = CreateIntParameter("R3", ControlMeta::kDefault,
                                       &params_.rates[2], 50, 0, 99, 1);
        r4_param_ = CreateIntParameter("R4", ControlMeta::kDefault,
                                       &params_.rates[3], 50, 0, 99, 1);
        ui.KnobInt("R1", r1_param_);
        ui.KnobInt("R2", r2_param_);
        ui.KnobInt("R3", r3_param_);
        ui.KnobInt("R4", r4_param_);
    }
    {
        auto _ = ui.HBox("EG Levels");
        l1_param_ = CreateIntParameter("L1", ControlMeta::kDefault,
                                       &params_.levels[0], 99, 0, 99, 1);
        l2_param_ = CreateIntParameter("L2", ControlMeta::kDefault,
                                       &params_.levels[1], 75, 0, 99, 1);
        l3_param_ = CreateIntParameter("L3", ControlMeta::kDefault,
                                       &params_.levels[2], 50, 0, 99, 1);
        l4_param_ = CreateIntParameter("L4", ControlMeta::kDefault,
                                       &params_.levels[3], 0, 0, 99, 1);
        ui.KnobInt("L1", l1_param_);
        ui.KnobInt("L2", l2_param_);
        ui.KnobInt("L3", l3_param_);
        ui.KnobInt("L4", l4_param_);
    }

    // Amp mod section
    {
        auto _ = ui.HBox("Amp Mod");
        amp_mod_sens_param_ = CreateIntParameter(
            "AMS", ControlMeta::kDefault, &params_.amp_mod_sens, 0, 0, 3, 1);
        amd_param_ = CreateIntParameter("AMD", ControlMeta::kDefault,
                                        &params_.lfo_amp_depth, 0, 0, 99, 1);
        velocity_sens_param_ =
            CreateIntParameter("Vel Sens", ControlMeta::kDefault,
                               &params_.velocity_sens, 0, 0, 7, 1);
        ui.KnobInt("AMS", amp_mod_sens_param_);
        ui.KnobInt("AMD", amd_param_);
        ui.KnobInt("Vel Sens", velocity_sens_param_);
    }
    // Pitch mod section
    {
        auto _ = ui.HBox("Pitch Mod");
        lfo_pitch_depth_param_ =
            CreateIntParameter("Pitch Depth", ControlMeta::kDefault,
                               &params_.lfo_pitch_depth, 0, 0, 99);
        pitch_mod_sens_param_ = CreateIntParameter(
            "PMS", ControlMeta::kDefault, &params_.pitch_mod_sens, 0, 0, 7);

        ui.KnobInt("Pitch Depth", lfo_pitch_depth_param_);
        ui.KnobInt("PMS", pitch_mod_sens_param_);
    }

    // "Keyboard Scaling" section:
    {
        auto _ = ui.HBox("Keyboard Scaling");
        kbd_rate_scaling_param_ =
            CreateIntParameter("Rate Scl", ControlMeta::kDefault,
                               &params_.kbd_rate_scaling, 0, 0, 7, 1);
        kbd_break_pt_param_ = CreateIntParameter(
            "Break Pt", ControlMeta::kDefault, &params_.kbd_break_pt, 0, 0, 99);
        kbd_left_depth_param_ =
            CreateIntParameter("Left Depth", ControlMeta::kDefault,
                               &params_.kbd_left_depth, 0, 0, 99);
        kbd_right_depth_param_ =
            CreateIntParameter("Right Depth", ControlMeta::kDefault,
                               &params_.kbd_right_depth, 0, 0, 99);
        kbd_left_curve_param_ =
            CreateIntParameter("Left Curve", ControlMeta::kDefault,
                               &params_.kbd_left_curve, 0, 0, 3);
        kbd_right_curve_param_ =
            CreateIntParameter("Right Curve", ControlMeta::kDefault,
                               &params_.kbd_right_curve, 0, 0, 3);

        ui.KnobInt("Rate Scl", kbd_rate_scaling_param_);
        ui.KnobInt("Break Pt", kbd_break_pt_param_);
        ui.KnobInt("Left Depth", kbd_left_depth_param_);
        ui.KnobInt("Right Depth", kbd_right_depth_param_);
        ui.KnobInt("Left Curve", kbd_left_curve_param_);
        ui.KnobInt("Right Curve", kbd_right_curve_param_);
    }
}

void Dexie::Process() {
    if (muted_) {
        audio_out_->Write(Audio());
        return;
    }
    const Audio pitch_buf = cv_pitch_in_->Read();
    const fy_real *pitch_data =
        pitch_buf.Empty() ? nullptr : pitch_buf.array().data();

    const Audio gate_buf = gate_in_->Read();
    const fy_real *gate_data =
        gate_buf.Empty() ? nullptr : gate_buf.array().data();

    const Audio phase_buf = cv_phase_in_->Read();
    const fy_real *phase_data =
        phase_buf.Empty() ? nullptr : phase_buf.array().data();

    const Audio amp_mod_buf = cv_amp_mod_in_->Read();
    const fy_real *amp_mod_data =
        amp_mod_buf.Empty() ? nullptr : amp_mod_buf.array().data();

    const Audio velocity_buf = cv_velocity_in_->Read();
    const fy_real *velocity_data =
        velocity_buf.Empty() ? nullptr : velocity_buf.array().data();

    const Audio pitch_mod_buf = cv_pitch_mod_in_->Read();
    const fy_real *pitch_mod_data =
        pitch_mod_buf.Empty() ? nullptr : pitch_mod_buf.array().data();

    const Audio pitch_env_buf = cv_pitch_env_in_->Read();
    const fy_real *pitch_env_data =
        pitch_env_buf.Empty() ? nullptr : pitch_env_buf.array().data();

    const float sample_rate = Audio::sample_rate();
    const std::size_t frames = Audio::frames();

    env_.SetSampleRate(sample_rate);

    // DX7 detune: ±7 steps → ±3.4 cents total (~0.486 cents/step).
    const float detune_cents = std::round(params_.detune) * (3.4f / 7.f);
    const float detune_factor = std::pow(2.f, detune_cents / 1200.f);

    const float ratio = ratio_coarse_ * (1 + ratio_fine_);

    // Feedback amount 0..7 → shift (FEEDBACK_BITDEPTH - amount), or 16 = off.
    const int feedback_shift =
        params_.feedback != 0 ? kFeedbackBitdepth - params_.feedback : 16;
    const bool fb_on = feedback_shift < 16;
    const float fb_divisor = static_cast<float>(1 << (feedback_shift + 1));

    const float ams_depth =
        kAmpModSensTab[std::clamp(params_.amp_mod_sens, 0, 3)];

    /*
    const int pitchmoddepth =
        (static_cast<int>(std::round(lfo_pitch_depth_)) * 165) >> 6;
    */
    const int pitchmoddepth = (params_.lfo_pitch_depth * 165) >> 6;
    /*
    const int pms_raw = kPitchModSensRaw[std::clamp(
        static_cast<int>(std::round(pitch_mod_sens_)), 0, 7)];
    */
    const int pms_raw =
        kPitchModSensRaw[std::clamp(params_.pitch_mod_sens, 0, 7)];
    const float pitch_mod_scale =
        static_cast<float>(pitchmoddepth * pms_raw) / 65536.0f;

    const float pitch_depth_norm = params_.lfo_pitch_depth / 99.0f;

    Audio out(ChannelLayout::kMono);
    fy_real *out_data = out.array().data();

    for (std::size_t i = 0; i < frames; ++i) {
        // --- Pitch CV (read first — needed by both the envelope's keyboard
        // level scaling below and the oscillator further down) ---
        const float pitch_cv =
            pitch_data ? static_cast<float>(pitch_data[i]) : 0.0f;

        const int midinote = 60 +
                             static_cast<int>(std::round(pitch_cv * 12.0f)) +
                             params_.transpose;

        // --- Envelope ---
        const bool gate = gate_data && (gate_data[i] > 0.5f);

        if (gate && !gate_prev_) {
            // Always clear feedback history — stale state from a previous note
            // bleeding into the new attack is never intentional on real
            // hardware.
            fb_hist_[0] = 0.0f;
            fb_hist_[1] = 0.0f;

            // Phase reset only when the patch requests it (osc_key_sync=1).
            // Free-running phase (osc_key_sync=0) is intentional DX7 behavior
            // giving slightly different timbre on each note-on.
            if (params_.osc_key_sync) {
                phase_ = 0.0f;
            }

            const int level_scaling =
                ScaleLevel(midinote, params_.kbd_break_pt,
                           params_.kbd_left_depth, params_.kbd_right_depth,
                           params_.kbd_left_curve, params_.kbd_right_curve);
            const float velocity_norm =
                velocity_data ? static_cast<float>(velocity_data[i]) : 1.0f;
            const int velocity_0_127 =
                static_cast<int>(std::round(velocity_norm * 127.0f));
            const int velocity_scaling =
                ScaleVelocity(velocity_0_127, params_.velocity_sens);
            const int rate_scaling =
                ScaleRate(midinote, params_.kbd_rate_scaling);

            env_.NoteOn(params_.rates, params_.levels, params_.output_level,
                        rate_scaling, level_scaling, velocity_scaling);
            samples_since_gate_on_ = 0;
        } else if (!gate && gate_prev_) {
            env_.NoteOff();
        }

        if (gate)
            ++samples_since_gate_on_;
        gate_prev_ = gate;

        const float env_amp = env_.Tick();

        // --- LFO amplitude modulation (tremolo) ---
        const float delay_ramp =
            lfo_delay_samples_total_ > 0
                ? std::clamp(static_cast<float>(samples_since_gate_on_) /
                                 static_cast<float>(lfo_delay_samples_total_),
                             0.0f, 1.0f)
                : 1.0f;

        const float lfo_val =
            amp_mod_data ? static_cast<float>(amp_mod_data[i]) : 0.0f;
        // LFO is bipolar [-1,1]; tremolo should only reduce level, never
        // boost it, so map to a [0,1] attenuation factor scaled by AMS,
        // patch-level depth, and the delay ramp-in.
        /*
        const float tremolo = AmpModLevelMultiplier(
            lfo_val, lfo_amp_depth_ / 99.0f, ams_depth, delay_ramp);
        */
        const float tremolo = AmpModLevelMultiplier(
            lfo_val, static_cast<float>(params_.lfo_amp_depth) / 99.0f,
            ams_depth, delay_ramp);

        // --- Oscillator ---
        const float phase_mod =
            phase_data ? static_cast<float>(phase_data[i]) : 0.0f;
        phase_mod_peak_ = std::max(phase_mod_peak_, std::fabs(phase_mod));

        // --- Pitch modulation (vibrato) ---
        // LFO signal scaled by voice-level pitch depth and per-voice pitch mod
        // sensitivity. lfo_val is bipolar [-1,1]; we convert to a frequency
        // multiplier via exp2 (small deviations only, so this is a good
        // approx). Dexed's fixed-point version adds pitch_mod directly to
        // logfreq; in float we achieve the same by multiplying frequency by
        // 2^(deviation).
        const float pitch_lfo =
            pitch_mod_data ? static_cast<float>(pitch_mod_data[i]) : 0.0f;
        const float pitch_eg =
            pitch_env_data ? static_cast<float>(pitch_env_data[i]) : 0.0f;

        // pitch_lfo is bipolar [-1,1]; delay_ramp fades in the LFO.
        // pitch_mod_scale converts to octaves matching Dexed's fixed-point
        // formula.
        const float pitch_lfo_oct = pitch_lfo * delay_ramp * pitch_mod_scale;
        const float pitch_factor = std::exp2(pitch_lfo_oct + pitch_eg);

        const float base_hz = CvToFreq(pitch_cv) * pitch_factor;
        const float op_hz = params_.fixed_freq ? frequency_ : (base_hz * ratio);

        // Detune applied to op_hz (post-ratio), matching osc_freq's behavior
        // where logfreq already includes coarsemul before detune is applied.
        // Fixed-frequency operators skip detune — the DX7 only adds detune in
        // ratio mode (osc_freq's fixed branch has a different, smaller detune
        // formula).

        const float freq =
            params_.fixed_freq
                ? op_hz
                : op_hz + DetuneHz(op_hz, std::round(params_.detune));

        debug_freq_ = freq;
        const float phase_inc = freq / sample_rate;

        const float fb =
            fb_on ? (fb_hist_[0] + fb_hist_[1]) / fb_divisor : 0.0f;

        const float sample = std::sin(kTwoPi * (phase_ + phase_mod + fb));
        float shaped = sample * env_amp * tremolo;
        shaped = ComputeFeedback(shaped, env_amp, fb_on);

        out_data[i] = static_cast<fy_real>(shaped);

        phase_ += phase_inc;
        if (phase_ >= 1.0f)
            phase_ -= std::floor(phase_);
    }

    audio_out_->Write(out);
}

void Dexie::RecomputeDerived() {
    ratio_coarse_ = CoarseToRatio(params_.coarse);
    ratio_fine_ = FineToRatioFine(params_.fine);
    frequency_ =
        params_.fixed_freq
            ? FixedFrequencyHz(params_.coarse, params_.fine, params_.detune)
            : 0.0f;
}

// dexie.cpp
float Dexie::ComputeFeedback(float shaped, float env_amp, bool fb_on) {
    if (!fb_on) {
        fb_hist_[1] = fb_hist_[0];
        fb_hist_[0] = shaped;
        return shaped;
    }

    /*
    const float feedback_norm = static_cast<float>(params_.feedback) / 7.0f;
    const float level_norm = output_level_ / 99.0f;
    const float noise_amount = feedback_norm * level_norm;
    */
    const float feedback_norm = static_cast<float>(params_.feedback) / 7.0f;
    const float level_norm = static_cast<float>(params_.output_level) / 99.0f;
    const float noise_amount = feedback_norm * level_norm;

    // constexpr float kNoiseThreshold = 0.75f;
    constexpr float kNoiseThreshold = 0.90f;
    if (noise_amount > kNoiseThreshold) {
        noise_seed_ = noise_seed_ * 1664525u + 1013904223u;
        const float noise =
            static_cast<float>((int32_t)noise_seed_) / 2147483648.0f;
        const float blend =
            (noise_amount - kNoiseThreshold) / (1.0f - kNoiseThreshold);
        shaped = shaped * (1.0f - blend) + noise * env_amp * blend;
    }

    fb_hist_[1] = fb_hist_[0];
    fb_hist_[0] = shaped;
    return shaped;
}

} // namespace augr::fm