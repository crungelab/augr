/* ------------------------------------------------------------
name: "chain"
Code generated with Faust 2.81.10 (https://faust.grame.fr)
Compilation options: -lang cpp -ct 1 -cn ChainDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0
------------------------------------------------------------ */

#ifndef  __ChainDsp_H__
#define  __ChainDsp_H__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif 

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <math.h>

#ifndef FAUSTCLASS 
#define FAUSTCLASS ChainDsp
#endif

#ifdef __APPLE__ 
#define exp10f __exp10f
#define exp10 __exp10
#endif

#if defined(_WIN32)
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif

static float ChainDsp_faustpower2_f(float value) {
	return value * value;
}
static float ChainDsp_faustpower4_f(float value) {
	return value * value * value * value;
}
static float ChainDsp_faustpower3_f(float value) {
	return value * value * value;
}

class ChainDsp : public augr::FaustDsp {
	
 private:
	
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	int fSampleRate;
	float fConst0;
	float fConst1;
	int iVec0[2];
	FAUSTFLOAT fVslider2;
	FAUSTFLOAT fVslider3;
	FAUSTFLOAT fVslider4;
	float fConst2;
	float fConst3;
	FAUSTFLOAT fVslider5;
	FAUSTFLOAT fVslider6;
	float fConst4;
	float fRec13[2];
	FAUSTFLOAT fVslider7;
	float fRec12[2];
	int IOTA0;
	float fVec1[16384];
	float fConst5;
	int iConst6;
	FAUSTFLOAT fCheckbox0;
	float fConst7;
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fCheckbox1;
	FAUSTFLOAT fCheckbox2;
	FAUSTFLOAT fCheckbox3;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	float fRec17[2];
	float fRec18[2];
	FAUSTFLOAT fHslider4;
	FAUSTFLOAT fHslider5;
	FAUSTFLOAT fHslider6;
	FAUSTFLOAT fHslider7;
	FAUSTFLOAT fHslider8;
	FAUSTFLOAT fCheckbox4;
	FAUSTFLOAT fHslider9;
	float fRec24[2];
	FAUSTFLOAT fCheckbox5;
	float fConst8;
	float fConst9;
	FAUSTFLOAT fHslider10;
	float fRec30[2];
	FAUSTFLOAT fHslider11;
	float fRec31[2];
	float fVec2[2];
	float fRec29[2];
	float fRec28[2];
	float fRec27[2];
	float fRec26[2];
	float fRec25[2];
	float fRec23[2];
	FAUSTFLOAT fHslider12;
	float fRec22[3];
	float fRec21[3];
	float fRec20[3];
	float fRec19[3];
	float fRec16[2];
	float fRec36[3];
	float fRec35[3];
	float fRec34[3];
	float fRec33[3];
	float fRec32[2];
	FAUSTFLOAT fHslider13;
	float fRec15[2];
	FAUSTFLOAT fHslider14;
	float fRec14[2];
	FAUSTFLOAT fHslider15;
	float fVec3[16384];
	FAUSTFLOAT fVslider8;
	float fConst10;
	float fVec4[4096];
	int iConst11;
	float fRec10[2];
	float fConst12;
	float fConst13;
	float fRec40[2];
	float fRec39[2];
	float fVec5[16384];
	float fConst14;
	int iConst15;
	float fVec6[2048];
	int iConst16;
	float fRec37[2];
	float fConst17;
	float fConst18;
	float fRec44[2];
	float fRec43[2];
	float fVec7[16384];
	float fConst19;
	int iConst20;
	float fVec8[4096];
	int iConst21;
	float fRec41[2];
	float fConst22;
	float fConst23;
	float fRec48[2];
	float fRec47[2];
	float fVec9[16384];
	float fConst24;
	int iConst25;
	float fVec10[2048];
	int iConst26;
	float fRec45[2];
	float fConst27;
	float fConst28;
	float fRec52[2];
	float fRec51[2];
	float fVec11[32768];
	float fConst29;
	int iConst30;
	float fRec54[2];
	float fRec53[2];
	FAUSTFLOAT fHbargraph0;
	float fVec12[16384];
	float fVec13[4096];
	int iConst31;
	float fRec49[2];
	float fConst32;
	float fConst33;
	float fRec58[2];
	float fRec57[2];
	float fVec14[16384];
	float fConst34;
	int iConst35;
	float fVec15[4096];
	int iConst36;
	float fRec55[2];
	float fConst37;
	float fConst38;
	float fRec62[2];
	float fRec61[2];
	float fVec16[32768];
	float fConst39;
	int iConst40;
	float fVec17[4096];
	int iConst41;
	float fRec59[2];
	float fConst42;
	float fConst43;
	float fRec66[2];
	float fRec65[2];
	float fVec18[32768];
	float fConst44;
	int iConst45;
	float fVec19[2048];
	int iConst46;
	float fRec63[2];
	float fRec2[3];
	float fRec3[3];
	float fRec4[3];
	float fRec5[3];
	float fRec6[3];
	float fRec7[3];
	float fRec8[3];
	float fRec9[3];
	float fRec1[3];
	float fRec0[3];
	FAUSTFLOAT fVslider9;
	float fRec67[2];
	FAUSTFLOAT fVslider10;
	float fRec68[2];
	float fRec70[3];
	float fRec69[3];
	
 public:
	ChainDsp() {
	}
	
	void metadata(Meta* m) { 
		m->declare("analyzers.lib/amp_follower_ar:author", "Jonatan Liljedahl, revised by Romain Michon");
		m->declare("analyzers.lib/name", "Faust Analyzer Library");
		m->declare("analyzers.lib/version", "1.3.0");
		m->declare("basics.lib/bypass1:author", "Julius Smith");
		m->declare("basics.lib/bypass2:author", "Julius Smith");
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "1.22.0");
		m->declare("compile_options", "-lang cpp -ct 1 -cn ChainDsp -scn augr::FaustDsp -es 1 -mcd 16 -mdd 1024 -mdy 33 -single -ftz 0");
		m->declare("compressors.lib/compression_gain_mono:author", "Julius O. Smith III");
		m->declare("compressors.lib/compression_gain_mono:copyright", "Copyright (C) 2014-2020 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("compressors.lib/compression_gain_mono:license", "MIT-style STK-4.3 license");
		m->declare("compressors.lib/compressor_stereo:author", "Julius O. Smith III");
		m->declare("compressors.lib/compressor_stereo:copyright", "Copyright (C) 2014-2020 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("compressors.lib/compressor_stereo:license", "MIT-style STK-4.3 license");
		m->declare("compressors.lib/name", "Faust Compressor Effect Library");
		m->declare("compressors.lib/version", "1.6.0");
		m->declare("delays.lib/name", "Faust Delay Library");
		m->declare("delays.lib/version", "1.2.0");
		m->declare("demos.lib/compressor_demo:author", "Julius O. Smith III");
		m->declare("demos.lib/compressor_demo:licence", "MIT");
		m->declare("demos.lib/cubicnl_demo:author", "Julius O. Smith III");
		m->declare("demos.lib/cubicnl_demo:licence", "MIT");
		m->declare("demos.lib/name", "Faust Demos Library");
		m->declare("demos.lib/phaser2_demo:author", "Julius O. Smith III");
		m->declare("demos.lib/phaser2_demo:licence", "MIT");
		m->declare("demos.lib/version", "1.2.0");
		m->declare("demos.lib/wah4_demo:author", "Julius O. Smith III");
		m->declare("demos.lib/wah4_demo:licence", "MIT");
		m->declare("demos.lib/zita_rev1:author", "Julius O. Smith III");
		m->declare("demos.lib/zita_rev1:licence", "MIT");
		m->declare("filename", "chain.dsp");
		m->declare("filters.lib/allpass_comb:author", "Julius O. Smith III");
		m->declare("filters.lib/allpass_comb:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/allpass_comb:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/dcblocker:author", "Julius O. Smith III");
		m->declare("filters.lib/dcblocker:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/dcblocker:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/fir:author", "Julius O. Smith III");
		m->declare("filters.lib/fir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/fir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/iir:author", "Julius O. Smith III");
		m->declare("filters.lib/iir:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/iir:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1", "MIT-style STK-4.3 license");
		m->declare("filters.lib/lowpass0_highpass1:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:author", "Julius O. Smith III");
		m->declare("filters.lib/lowpass:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/lowpass:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/nlf2:author", "Julius O. Smith III");
		m->declare("filters.lib/nlf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/nlf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/peak_eq_rm:author", "Julius O. Smith III");
		m->declare("filters.lib/peak_eq_rm:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/peak_eq_rm:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/pole:author", "Julius O. Smith III");
		m->declare("filters.lib/pole:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/pole:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf1s:author", "Julius O. Smith III");
		m->declare("filters.lib/tf1s:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf1s:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/tf2:author", "Julius O. Smith III");
		m->declare("filters.lib/tf2:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/tf2:license", "MIT-style STK-4.3 license");
		m->declare("filters.lib/version", "1.7.1");
		m->declare("filters.lib/zero:author", "Julius O. Smith III");
		m->declare("filters.lib/zero:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("filters.lib/zero:license", "MIT-style STK-4.3 license");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.9.0");
		m->declare("misceffects.lib/cubicnl:author", "Julius O. Smith III");
		m->declare("misceffects.lib/cubicnl:license", "STK-4.3");
		m->declare("misceffects.lib/cubicnl_nodc:author", "Julius O. Smith III");
		m->declare("misceffects.lib/cubicnl_nodc:license", "STK-4.3");
		m->declare("misceffects.lib/name", "Misc Effects Library");
		m->declare("misceffects.lib/version", "2.5.1");
		m->declare("name", "chain");
		m->declare("oscillators.lib/name", "Faust Oscillator Library");
		m->declare("oscillators.lib/version", "1.6.0");
		m->declare("phaflangers.lib/name", "Faust Phaser and Flanger Library");
		m->declare("phaflangers.lib/version", "1.1.0");
		m->declare("platform.lib/name", "Generic Platform Library");
		m->declare("platform.lib/version", "1.3.0");
		m->declare("reverbs.lib/name", "Faust Reverb Library");
		m->declare("reverbs.lib/version", "1.4.0");
		m->declare("routes.lib/hadamard:author", "Remy Muller, revised by Romain Michon");
		m->declare("routes.lib/name", "Faust Signal Routing Library");
		m->declare("routes.lib/version", "1.2.0");
		m->declare("signals.lib/name", "Faust Signal Routing Library");
		m->declare("signals.lib/onePoleSwitching:author", "Jonatan Liljedahl, revised by Dario Sanfilippo");
		m->declare("signals.lib/onePoleSwitching:licence", "STK-4.3");
		m->declare("signals.lib/version", "1.6.0");
		m->declare("vaeffects.lib/moog_vcf:author", "Julius O. Smith III");
		m->declare("vaeffects.lib/moog_vcf:copyright", "Copyright (C) 2003-2019 by Julius O. Smith III <jos@ccrma.stanford.edu>");
		m->declare("vaeffects.lib/moog_vcf:license", "MIT-style STK-4.3 license");
		m->declare("vaeffects.lib/name", "Faust Virtual Analog Filter Effect Library");
		m->declare("vaeffects.lib/version", "1.4.0");
	}

	virtual int getNumInputs() {
		return 1;
	}
	virtual int getNumOutputs() {
		return 2;
	}
	
	static void classInit(int sample_rate) {
	}
	
	virtual void instanceConstants(int sample_rate) {
		fSampleRate = sample_rate;
		fConst0 = std::min<float>(1.92e+05f, std::max<float>(1.0f, static_cast<float>(fSampleRate)));
		fConst1 = 6.2831855f / fConst0;
		fConst2 = std::floor(0.174713f * fConst0 + 0.5f);
		fConst3 = 6.9077554f * (fConst2 / fConst0);
		fConst4 = 3.1415927f / fConst0;
		fConst5 = std::floor(0.022904f * fConst0 + 0.5f);
		iConst6 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst2 - fConst5)));
		fConst7 = 1.0f / fConst0;
		fConst8 = 44.1f / fConst0;
		fConst9 = 1.0f - fConst8;
		fConst10 = 0.001f * fConst0;
		iConst11 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst5 + -1.0f)));
		fConst12 = std::floor(0.153129f * fConst0 + 0.5f);
		fConst13 = 6.9077554f * (fConst12 / fConst0);
		fConst14 = std::floor(0.020346f * fConst0 + 0.5f);
		iConst15 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst12 - fConst14)));
		iConst16 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, fConst14 + -1.0f)));
		fConst17 = std::floor(0.127837f * fConst0 + 0.5f);
		fConst18 = 6.9077554f * (fConst17 / fConst0);
		fConst19 = std::floor(0.031604f * fConst0 + 0.5f);
		iConst20 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst17 - fConst19)));
		iConst21 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst19 + -1.0f)));
		fConst22 = std::floor(0.125f * fConst0 + 0.5f);
		fConst23 = 6.9077554f * (fConst22 / fConst0);
		fConst24 = std::floor(0.013458f * fConst0 + 0.5f);
		iConst25 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst22 - fConst24)));
		iConst26 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, fConst24 + -1.0f)));
		fConst27 = std::floor(0.210389f * fConst0 + 0.5f);
		fConst28 = 6.9077554f * (fConst27 / fConst0);
		fConst29 = std::floor(0.024421f * fConst0 + 0.5f);
		iConst30 = static_cast<int>(std::min<float>(16384.0f, std::max<float>(0.0f, fConst27 - fConst29)));
		iConst31 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst29 + -1.0f)));
		fConst32 = std::floor(0.192303f * fConst0 + 0.5f);
		fConst33 = 6.9077554f * (fConst32 / fConst0);
		fConst34 = std::floor(0.029291f * fConst0 + 0.5f);
		iConst35 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst32 - fConst34)));
		iConst36 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst34 + -1.0f)));
		fConst37 = std::floor(0.256891f * fConst0 + 0.5f);
		fConst38 = 6.9077554f * (fConst37 / fConst0);
		fConst39 = std::floor(0.027333f * fConst0 + 0.5f);
		iConst40 = static_cast<int>(std::min<float>(16384.0f, std::max<float>(0.0f, fConst37 - fConst39)));
		iConst41 = static_cast<int>(std::min<float>(2048.0f, std::max<float>(0.0f, fConst39 + -1.0f)));
		fConst42 = std::floor(0.219991f * fConst0 + 0.5f);
		fConst43 = 6.9077554f * (fConst42 / fConst0);
		fConst44 = std::floor(0.019123f * fConst0 + 0.5f);
		iConst45 = static_cast<int>(std::min<float>(16384.0f, std::max<float>(0.0f, fConst42 - fConst44)));
		iConst46 = static_cast<int>(std::min<float>(1024.0f, std::max<float>(0.0f, fConst44 + -1.0f)));
	}
	
	virtual void instanceResetUserInterface() {
		fVslider0 = static_cast<FAUSTFLOAT>(0.0f);
		fVslider1 = static_cast<FAUSTFLOAT>(1.5e+03f);
		fVslider2 = static_cast<FAUSTFLOAT>(0.0f);
		fVslider3 = static_cast<FAUSTFLOAT>(315.0f);
		fVslider4 = static_cast<FAUSTFLOAT>(2.0f);
		fVslider5 = static_cast<FAUSTFLOAT>(6e+03f);
		fVslider6 = static_cast<FAUSTFLOAT>(2e+02f);
		fVslider7 = static_cast<FAUSTFLOAT>(3.0f);
		fCheckbox0 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider0 = static_cast<FAUSTFLOAT>(5e+01f);
		fHslider1 = static_cast<FAUSTFLOAT>(-3e+01f);
		fCheckbox1 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox2 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox3 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider2 = static_cast<FAUSTFLOAT>(1.0f);
		fHslider3 = static_cast<FAUSTFLOAT>(0.5f);
		fHslider4 = static_cast<FAUSTFLOAT>(1e+02f);
		fHslider5 = static_cast<FAUSTFLOAT>(8e+02f);
		fHslider6 = static_cast<FAUSTFLOAT>(1.5f);
		fHslider7 = static_cast<FAUSTFLOAT>(1e+03f);
		fHslider8 = static_cast<FAUSTFLOAT>(0.0f);
		fCheckbox4 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider9 = static_cast<FAUSTFLOAT>(2e+02f);
		fCheckbox5 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider10 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider11 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider12 = static_cast<FAUSTFLOAT>(0.0f);
		fHslider13 = static_cast<FAUSTFLOAT>(5e+02f);
		fHslider14 = static_cast<FAUSTFLOAT>(5.0f);
		fHslider15 = static_cast<FAUSTFLOAT>(4e+01f);
		fVslider8 = static_cast<FAUSTFLOAT>(6e+01f);
		fVslider9 = static_cast<FAUSTFLOAT>(0.0f);
		fVslider10 = static_cast<FAUSTFLOAT>(-2e+01f);
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
			iVec0[l0] = 0;
		}
		for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
			fRec13[l1] = 0.0f;
		}
		for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
			fRec12[l2] = 0.0f;
		}
		IOTA0 = 0;
		for (int l3 = 0; l3 < 16384; l3 = l3 + 1) {
			fVec1[l3] = 0.0f;
		}
		for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
			fRec17[l4] = 0.0f;
		}
		for (int l5 = 0; l5 < 2; l5 = l5 + 1) {
			fRec18[l5] = 0.0f;
		}
		for (int l6 = 0; l6 < 2; l6 = l6 + 1) {
			fRec24[l6] = 0.0f;
		}
		for (int l7 = 0; l7 < 2; l7 = l7 + 1) {
			fRec30[l7] = 0.0f;
		}
		for (int l8 = 0; l8 < 2; l8 = l8 + 1) {
			fRec31[l8] = 0.0f;
		}
		for (int l9 = 0; l9 < 2; l9 = l9 + 1) {
			fVec2[l9] = 0.0f;
		}
		for (int l10 = 0; l10 < 2; l10 = l10 + 1) {
			fRec29[l10] = 0.0f;
		}
		for (int l11 = 0; l11 < 2; l11 = l11 + 1) {
			fRec28[l11] = 0.0f;
		}
		for (int l12 = 0; l12 < 2; l12 = l12 + 1) {
			fRec27[l12] = 0.0f;
		}
		for (int l13 = 0; l13 < 2; l13 = l13 + 1) {
			fRec26[l13] = 0.0f;
		}
		for (int l14 = 0; l14 < 2; l14 = l14 + 1) {
			fRec25[l14] = 0.0f;
		}
		for (int l15 = 0; l15 < 2; l15 = l15 + 1) {
			fRec23[l15] = 0.0f;
		}
		for (int l16 = 0; l16 < 3; l16 = l16 + 1) {
			fRec22[l16] = 0.0f;
		}
		for (int l17 = 0; l17 < 3; l17 = l17 + 1) {
			fRec21[l17] = 0.0f;
		}
		for (int l18 = 0; l18 < 3; l18 = l18 + 1) {
			fRec20[l18] = 0.0f;
		}
		for (int l19 = 0; l19 < 3; l19 = l19 + 1) {
			fRec19[l19] = 0.0f;
		}
		for (int l20 = 0; l20 < 2; l20 = l20 + 1) {
			fRec16[l20] = 0.0f;
		}
		for (int l21 = 0; l21 < 3; l21 = l21 + 1) {
			fRec36[l21] = 0.0f;
		}
		for (int l22 = 0; l22 < 3; l22 = l22 + 1) {
			fRec35[l22] = 0.0f;
		}
		for (int l23 = 0; l23 < 3; l23 = l23 + 1) {
			fRec34[l23] = 0.0f;
		}
		for (int l24 = 0; l24 < 3; l24 = l24 + 1) {
			fRec33[l24] = 0.0f;
		}
		for (int l25 = 0; l25 < 2; l25 = l25 + 1) {
			fRec32[l25] = 0.0f;
		}
		for (int l26 = 0; l26 < 2; l26 = l26 + 1) {
			fRec15[l26] = 0.0f;
		}
		for (int l27 = 0; l27 < 2; l27 = l27 + 1) {
			fRec14[l27] = 0.0f;
		}
		for (int l28 = 0; l28 < 16384; l28 = l28 + 1) {
			fVec3[l28] = 0.0f;
		}
		for (int l29 = 0; l29 < 4096; l29 = l29 + 1) {
			fVec4[l29] = 0.0f;
		}
		for (int l30 = 0; l30 < 2; l30 = l30 + 1) {
			fRec10[l30] = 0.0f;
		}
		for (int l31 = 0; l31 < 2; l31 = l31 + 1) {
			fRec40[l31] = 0.0f;
		}
		for (int l32 = 0; l32 < 2; l32 = l32 + 1) {
			fRec39[l32] = 0.0f;
		}
		for (int l33 = 0; l33 < 16384; l33 = l33 + 1) {
			fVec5[l33] = 0.0f;
		}
		for (int l34 = 0; l34 < 2048; l34 = l34 + 1) {
			fVec6[l34] = 0.0f;
		}
		for (int l35 = 0; l35 < 2; l35 = l35 + 1) {
			fRec37[l35] = 0.0f;
		}
		for (int l36 = 0; l36 < 2; l36 = l36 + 1) {
			fRec44[l36] = 0.0f;
		}
		for (int l37 = 0; l37 < 2; l37 = l37 + 1) {
			fRec43[l37] = 0.0f;
		}
		for (int l38 = 0; l38 < 16384; l38 = l38 + 1) {
			fVec7[l38] = 0.0f;
		}
		for (int l39 = 0; l39 < 4096; l39 = l39 + 1) {
			fVec8[l39] = 0.0f;
		}
		for (int l40 = 0; l40 < 2; l40 = l40 + 1) {
			fRec41[l40] = 0.0f;
		}
		for (int l41 = 0; l41 < 2; l41 = l41 + 1) {
			fRec48[l41] = 0.0f;
		}
		for (int l42 = 0; l42 < 2; l42 = l42 + 1) {
			fRec47[l42] = 0.0f;
		}
		for (int l43 = 0; l43 < 16384; l43 = l43 + 1) {
			fVec9[l43] = 0.0f;
		}
		for (int l44 = 0; l44 < 2048; l44 = l44 + 1) {
			fVec10[l44] = 0.0f;
		}
		for (int l45 = 0; l45 < 2; l45 = l45 + 1) {
			fRec45[l45] = 0.0f;
		}
		for (int l46 = 0; l46 < 2; l46 = l46 + 1) {
			fRec52[l46] = 0.0f;
		}
		for (int l47 = 0; l47 < 2; l47 = l47 + 1) {
			fRec51[l47] = 0.0f;
		}
		for (int l48 = 0; l48 < 32768; l48 = l48 + 1) {
			fVec11[l48] = 0.0f;
		}
		for (int l49 = 0; l49 < 2; l49 = l49 + 1) {
			fRec54[l49] = 0.0f;
		}
		for (int l50 = 0; l50 < 2; l50 = l50 + 1) {
			fRec53[l50] = 0.0f;
		}
		for (int l51 = 0; l51 < 16384; l51 = l51 + 1) {
			fVec12[l51] = 0.0f;
		}
		for (int l52 = 0; l52 < 4096; l52 = l52 + 1) {
			fVec13[l52] = 0.0f;
		}
		for (int l53 = 0; l53 < 2; l53 = l53 + 1) {
			fRec49[l53] = 0.0f;
		}
		for (int l54 = 0; l54 < 2; l54 = l54 + 1) {
			fRec58[l54] = 0.0f;
		}
		for (int l55 = 0; l55 < 2; l55 = l55 + 1) {
			fRec57[l55] = 0.0f;
		}
		for (int l56 = 0; l56 < 16384; l56 = l56 + 1) {
			fVec14[l56] = 0.0f;
		}
		for (int l57 = 0; l57 < 4096; l57 = l57 + 1) {
			fVec15[l57] = 0.0f;
		}
		for (int l58 = 0; l58 < 2; l58 = l58 + 1) {
			fRec55[l58] = 0.0f;
		}
		for (int l59 = 0; l59 < 2; l59 = l59 + 1) {
			fRec62[l59] = 0.0f;
		}
		for (int l60 = 0; l60 < 2; l60 = l60 + 1) {
			fRec61[l60] = 0.0f;
		}
		for (int l61 = 0; l61 < 32768; l61 = l61 + 1) {
			fVec16[l61] = 0.0f;
		}
		for (int l62 = 0; l62 < 4096; l62 = l62 + 1) {
			fVec17[l62] = 0.0f;
		}
		for (int l63 = 0; l63 < 2; l63 = l63 + 1) {
			fRec59[l63] = 0.0f;
		}
		for (int l64 = 0; l64 < 2; l64 = l64 + 1) {
			fRec66[l64] = 0.0f;
		}
		for (int l65 = 0; l65 < 2; l65 = l65 + 1) {
			fRec65[l65] = 0.0f;
		}
		for (int l66 = 0; l66 < 32768; l66 = l66 + 1) {
			fVec18[l66] = 0.0f;
		}
		for (int l67 = 0; l67 < 2048; l67 = l67 + 1) {
			fVec19[l67] = 0.0f;
		}
		for (int l68 = 0; l68 < 2; l68 = l68 + 1) {
			fRec63[l68] = 0.0f;
		}
		for (int l69 = 0; l69 < 3; l69 = l69 + 1) {
			fRec2[l69] = 0.0f;
		}
		for (int l70 = 0; l70 < 3; l70 = l70 + 1) {
			fRec3[l70] = 0.0f;
		}
		for (int l71 = 0; l71 < 3; l71 = l71 + 1) {
			fRec4[l71] = 0.0f;
		}
		for (int l72 = 0; l72 < 3; l72 = l72 + 1) {
			fRec5[l72] = 0.0f;
		}
		for (int l73 = 0; l73 < 3; l73 = l73 + 1) {
			fRec6[l73] = 0.0f;
		}
		for (int l74 = 0; l74 < 3; l74 = l74 + 1) {
			fRec7[l74] = 0.0f;
		}
		for (int l75 = 0; l75 < 3; l75 = l75 + 1) {
			fRec8[l75] = 0.0f;
		}
		for (int l76 = 0; l76 < 3; l76 = l76 + 1) {
			fRec9[l76] = 0.0f;
		}
		for (int l77 = 0; l77 < 3; l77 = l77 + 1) {
			fRec1[l77] = 0.0f;
		}
		for (int l78 = 0; l78 < 3; l78 = l78 + 1) {
			fRec0[l78] = 0.0f;
		}
		for (int l79 = 0; l79 < 2; l79 = l79 + 1) {
			fRec67[l79] = 0.0f;
		}
		for (int l80 = 0; l80 < 2; l80 = l80 + 1) {
			fRec68[l80] = 0.0f;
		}
		for (int l81 = 0; l81 < 3; l81 = l81 + 1) {
			fRec70[l81] = 0.0f;
		}
		for (int l82 = 0; l82 < 3; l82 = l82 + 1) {
			fRec69[l82] = 0.0f;
		}
	}
	
	virtual void init(int sample_rate) {
		classInit(sample_rate);
		instanceInit(sample_rate);
	}
	
	virtual void instanceInit(int sample_rate) {
		instanceConstants(sample_rate);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual ChainDsp* clone() {
		return new ChainDsp();
	}
	
	virtual int getSampleRate() {
		return fSampleRate;
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("chain");
		ui_interface->declare(0, "tooltip", "Reference:         http://en.wikipedia.org/wiki/Dynamic_range_compression");
		ui_interface->openVerticalBox("COMPRESSOR");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fCheckbox0, "0", "");
		ui_interface->declare(&fCheckbox0, "tooltip", "When this is checked, the compressor         has no effect");
		ui_interface->addCheckButton("Bypass", &fCheckbox0);
		ui_interface->declare(&fHbargraph0, "1", "");
		ui_interface->declare(&fHbargraph0, "tooltip", "Current gain of     the compressor in dB");
		ui_interface->declare(&fHbargraph0, "unit", "dB");
		ui_interface->addHorizontalBargraph("Compressor Gain Before Makeup", &fHbargraph0, FAUSTFLOAT(-5e+01f), FAUSTFLOAT(1e+01f));
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(0, "3", "");
		ui_interface->openHorizontalBox("Compression Control");
		ui_interface->declare(&fHslider14, "0", "");
		ui_interface->declare(&fHslider14, "style", "knob");
		ui_interface->declare(&fHslider14, "tooltip", "A compression Ratio of N means that for each N dB increase in input     signal level above Threshold, the output level goes up 1 dB");
		ui_interface->addHorizontalSlider("Ratio", &fHslider14, FAUSTFLOAT(5.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fHslider1, "1", "");
		ui_interface->declare(&fHslider1, "style", "knob");
		ui_interface->declare(&fHslider1, "tooltip", "When the signal level exceeds the Threshold (in dB), its level     is compressed according to the Ratio");
		ui_interface->declare(&fHslider1, "unit", "dB");
		ui_interface->addHorizontalSlider("Threshold", &fHslider1, FAUSTFLOAT(-3e+01f), FAUSTFLOAT(-1e+02f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->declare(0, "4", "");
		ui_interface->openHorizontalBox("Compression Response");
		ui_interface->declare(&fHslider0, "1", "");
		ui_interface->declare(&fHslider0, "scale", "log");
		ui_interface->declare(&fHslider0, "style", "knob");
		ui_interface->declare(&fHslider0, "tooltip", "Time constant in ms (1/e smoothing time) for the compression gain     to approach (exponentially) a new lower target level (the compression     `kicking in')");
		ui_interface->declare(&fHslider0, "unit", "ms");
		ui_interface->addHorizontalSlider("Attack", &fHslider0, FAUSTFLOAT(5e+01f), FAUSTFLOAT(1.0f), FAUSTFLOAT(1e+03f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fHslider13, "2", "");
		ui_interface->declare(&fHslider13, "scale", "log");
		ui_interface->declare(&fHslider13, "style", "knob");
		ui_interface->declare(&fHslider13, "tooltip", "Time constant in ms (1/e smoothing time) for the compression gain     to approach (exponentially) a new higher target level (the compression     'releasing')");
		ui_interface->declare(&fHslider13, "unit", "ms");
		ui_interface->addHorizontalSlider("Release", &fHslider13, FAUSTFLOAT(5e+02f), FAUSTFLOAT(1.0f), FAUSTFLOAT(1e+03f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(&fHslider15, "5", "");
		ui_interface->declare(&fHslider15, "tooltip", "The compressed-signal output level is increased by this amount     (in dB) to make up for the level lost due to compression");
		ui_interface->declare(&fHslider15, "unit", "dB");
		ui_interface->addHorizontalSlider("Makeup Gain", &fHslider15, FAUSTFLOAT(4e+01f), FAUSTFLOAT(-96.0f), FAUSTFLOAT(96.0f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->declare(0, "tooltip", "Reference:         https://ccrma.stanford.edu/~jos/pasp/Cubic_Soft_Clipper.html");
		ui_interface->openVerticalBox("CUBIC NONLINEARITY cubicnl");
		ui_interface->declare(&fCheckbox5, "0", "");
		ui_interface->declare(&fCheckbox5, "tooltip", "When this is checked, the         nonlinearity has no effect");
		ui_interface->addCheckButton("Bypass", &fCheckbox5);
		ui_interface->declare(&fHslider10, "1", "");
		ui_interface->declare(&fHslider10, "tooltip", "Amount of distortion");
		ui_interface->addHorizontalSlider("Drive", &fHslider10, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fHslider11, "2", "");
		ui_interface->declare(&fHslider11, "tooltip", "Brings in even harmonics");
		ui_interface->addHorizontalSlider("Offset", &fHslider11, FAUSTFLOAT(0.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->closeBox();
		ui_interface->declare(0, "tooltip", "Reference:         https://ccrma.stanford.edu/~jos/pasp/Flanging.html");
		ui_interface->openVerticalBox("PHASER2");
		ui_interface->declare(0, "0", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fCheckbox1, "0", "");
		ui_interface->declare(&fCheckbox1, "tooltip", "When this is checked, the phaser         has no effect");
		ui_interface->addCheckButton("Bypass", &fCheckbox1);
		ui_interface->declare(&fCheckbox2, "1", "");
		ui_interface->addCheckButton("Invert Internal Phaser Sum", &fCheckbox2);
		ui_interface->declare(&fCheckbox3, "2", "");
		ui_interface->addCheckButton("Vibrato Mode", &fCheckbox3);
		ui_interface->closeBox();
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider3, "1", "");
		ui_interface->declare(&fHslider3, "style", "knob");
		ui_interface->declare(&fHslider3, "unit", "Hz");
		ui_interface->addHorizontalSlider("Speed", &fHslider3, FAUSTFLOAT(0.5f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fHslider2, "2", "");
		ui_interface->declare(&fHslider2, "style", "knob");
		ui_interface->addHorizontalSlider("Notch Depth (Intensity)", &fHslider2, FAUSTFLOAT(1.0f), FAUSTFLOAT(0.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.001f));
		ui_interface->declare(&fHslider8, "3", "");
		ui_interface->declare(&fHslider8, "style", "knob");
		ui_interface->addHorizontalSlider("Feedback Gain", &fHslider8, FAUSTFLOAT(0.0f), FAUSTFLOAT(-0.999f), FAUSTFLOAT(0.999f), FAUSTFLOAT(0.001f));
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider7, "1", "");
		ui_interface->declare(&fHslider7, "scale", "log");
		ui_interface->declare(&fHslider7, "style", "knob");
		ui_interface->declare(&fHslider7, "unit", "Hz");
		ui_interface->addHorizontalSlider("Notch width", &fHslider7, FAUSTFLOAT(1e+03f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(5e+03f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider4, "2", "");
		ui_interface->declare(&fHslider4, "scale", "log");
		ui_interface->declare(&fHslider4, "style", "knob");
		ui_interface->declare(&fHslider4, "unit", "Hz");
		ui_interface->addHorizontalSlider("Min Notch1 Freq", &fHslider4, FAUSTFLOAT(1e+02f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(5e+03f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider5, "3", "");
		ui_interface->declare(&fHslider5, "scale", "log");
		ui_interface->declare(&fHslider5, "style", "knob");
		ui_interface->declare(&fHslider5, "unit", "Hz");
		ui_interface->addHorizontalSlider("Max Notch1 Freq", &fHslider5, FAUSTFLOAT(8e+02f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(1e+04f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fHslider6, "4", "");
		ui_interface->declare(&fHslider6, "style", "knob");
		ui_interface->addHorizontalSlider("Notch Freq Ratio: NotchFreq(n+1)/NotchFreq(n)", &fHslider6, FAUSTFLOAT(1.5f), FAUSTFLOAT(1.1f), FAUSTFLOAT(4.0f), FAUSTFLOAT(0.001f));
		ui_interface->closeBox();
		ui_interface->declare(0, "3", "");
		ui_interface->openHorizontalBox("0x00");
		ui_interface->declare(&fHslider12, "unit", "dB");
		ui_interface->addHorizontalSlider("Phaser Output Level", &fHslider12, FAUSTFLOAT(0.0f), FAUSTFLOAT(-6e+01f), FAUSTFLOAT(1e+01f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->declare(0, "tooltip", "Fourth-order wah effect made using moog_vcf");
		ui_interface->openHorizontalBox("WAH4");
		ui_interface->declare(&fCheckbox4, "0", "");
		ui_interface->declare(&fCheckbox4, "tooltip", "When this is checked, the wah pedal has         no effect");
		ui_interface->addCheckButton("Bypass", &fCheckbox4);
		ui_interface->declare(&fHslider9, "1", "");
		ui_interface->declare(&fHslider9, "scale", "log");
		ui_interface->declare(&fHslider9, "tooltip", "wah resonance         frequency in Hz");
		ui_interface->addHorizontalSlider("Resonance Frequency", &fHslider9, FAUSTFLOAT(2e+02f), FAUSTFLOAT(1e+02f), FAUSTFLOAT(2e+03f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->declare(0, "0", "");
		ui_interface->declare(0, "tooltip", "~ ZITA REV1 FEEDBACK DELAY NETWORK (FDN) & SCHROEDER     ALLPASS-COMB REVERBERATOR (8x8). See Faust's reverbs.lib for documentation and     references");
		ui_interface->openHorizontalBox("Zita_Rev1");
		ui_interface->declare(0, "1", "");
		ui_interface->openHorizontalBox("Input");
		ui_interface->declare(&fVslider8, "1", "");
		ui_interface->declare(&fVslider8, "style", "knob");
		ui_interface->declare(&fVslider8, "tooltip", "Delay in ms         before reverberation begins");
		ui_interface->declare(&fVslider8, "unit", "ms");
		ui_interface->addVerticalSlider("In Delay", &fVslider8, FAUSTFLOAT(6e+01f), FAUSTFLOAT(2e+01f), FAUSTFLOAT(1e+02f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->declare(0, "2", "");
		ui_interface->openHorizontalBox("Decay Times in Bands (see tooltips)");
		ui_interface->declare(&fVslider6, "1", "");
		ui_interface->declare(&fVslider6, "scale", "log");
		ui_interface->declare(&fVslider6, "style", "knob");
		ui_interface->declare(&fVslider6, "tooltip", "Crossover frequency (Hz) separating low and middle frequencies");
		ui_interface->declare(&fVslider6, "unit", "Hz");
		ui_interface->addVerticalSlider("LF X", &fVslider6, FAUSTFLOAT(2e+02f), FAUSTFLOAT(5e+01f), FAUSTFLOAT(1e+03f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fVslider7, "2", "");
		ui_interface->declare(&fVslider7, "scale", "log");
		ui_interface->declare(&fVslider7, "style", "knob");
		ui_interface->declare(&fVslider7, "tooltip", "T60 = time (in seconds) to decay 60dB in low-frequency band");
		ui_interface->declare(&fVslider7, "unit", "s");
		ui_interface->addVerticalSlider("Low RT60", &fVslider7, FAUSTFLOAT(3.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(8.0f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fVslider4, "3", "");
		ui_interface->declare(&fVslider4, "scale", "log");
		ui_interface->declare(&fVslider4, "style", "knob");
		ui_interface->declare(&fVslider4, "tooltip", "T60 = time (in seconds) to decay 60dB in middle band");
		ui_interface->declare(&fVslider4, "unit", "s");
		ui_interface->addVerticalSlider("Mid RT60", &fVslider4, FAUSTFLOAT(2.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(8.0f), FAUSTFLOAT(0.1f));
		ui_interface->declare(&fVslider5, "4", "");
		ui_interface->declare(&fVslider5, "scale", "log");
		ui_interface->declare(&fVslider5, "style", "knob");
		ui_interface->declare(&fVslider5, "tooltip", "Frequency (Hz) at which the high-frequency T60 is half the middle-band's T60");
		ui_interface->declare(&fVslider5, "unit", "Hz");
		ui_interface->addVerticalSlider("HF Damping", &fVslider5, FAUSTFLOAT(6e+03f), FAUSTFLOAT(1.5e+03f), FAUSTFLOAT(2.352e+04f), FAUSTFLOAT(1.0f));
		ui_interface->closeBox();
		ui_interface->declare(0, "3", "");
		ui_interface->openHorizontalBox("RM Peaking Equalizer 1");
		ui_interface->declare(&fVslider3, "1", "");
		ui_interface->declare(&fVslider3, "scale", "log");
		ui_interface->declare(&fVslider3, "style", "knob");
		ui_interface->declare(&fVslider3, "tooltip", "Center-frequency of second-order Regalia-Mitra peaking equalizer section 1");
		ui_interface->declare(&fVslider3, "unit", "Hz");
		ui_interface->addVerticalSlider("Eq1 Freq", &fVslider3, FAUSTFLOAT(315.0f), FAUSTFLOAT(4e+01f), FAUSTFLOAT(2.5e+03f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fVslider2, "2", "");
		ui_interface->declare(&fVslider2, "style", "knob");
		ui_interface->declare(&fVslider2, "tooltip", "Peak level         in dB of second-order Regalia-Mitra peaking equalizer section 1");
		ui_interface->declare(&fVslider2, "unit", "dB");
		ui_interface->addVerticalSlider("Eq1 Level", &fVslider2, FAUSTFLOAT(0.0f), FAUSTFLOAT(-15.0f), FAUSTFLOAT(15.0f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->declare(0, "4", "");
		ui_interface->openHorizontalBox("RM Peaking Equalizer 2");
		ui_interface->declare(&fVslider1, "1", "");
		ui_interface->declare(&fVslider1, "scale", "log");
		ui_interface->declare(&fVslider1, "style", "knob");
		ui_interface->declare(&fVslider1, "tooltip", "Center-frequency of second-order Regalia-Mitra peaking equalizer section 2");
		ui_interface->declare(&fVslider1, "unit", "Hz");
		ui_interface->addVerticalSlider("Eq2 Freq", &fVslider1, FAUSTFLOAT(1.5e+03f), FAUSTFLOAT(1.6e+02f), FAUSTFLOAT(1e+04f), FAUSTFLOAT(1.0f));
		ui_interface->declare(&fVslider0, "2", "");
		ui_interface->declare(&fVslider0, "style", "knob");
		ui_interface->declare(&fVslider0, "tooltip", "Peak level         in dB of second-order Regalia-Mitra peaking equalizer section 2");
		ui_interface->declare(&fVslider0, "unit", "dB");
		ui_interface->addVerticalSlider("Eq2 Level", &fVslider0, FAUSTFLOAT(0.0f), FAUSTFLOAT(-15.0f), FAUSTFLOAT(15.0f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->declare(0, "5", "");
		ui_interface->openHorizontalBox("Output");
		ui_interface->declare(&fVslider9, "1", "");
		ui_interface->declare(&fVslider9, "style", "knob");
		ui_interface->declare(&fVslider9, "tooltip", "Ratio of dry and wet signal. -1 = fully wet, +1 = fully dry");
		ui_interface->addVerticalSlider("Wet/Dry Mix", &fVslider9, FAUSTFLOAT(0.0f), FAUSTFLOAT(-1.0f), FAUSTFLOAT(1.0f), FAUSTFLOAT(0.01f));
		ui_interface->declare(&fVslider10, "2", "");
		ui_interface->declare(&fVslider10, "style", "knob");
		ui_interface->declare(&fVslider10, "tooltip", "Output scale         factor");
		ui_interface->declare(&fVslider10, "unit", "dB");
		ui_interface->addVerticalSlider("Level", &fVslider10, FAUSTFLOAT(-2e+01f), FAUSTFLOAT(-7e+01f), FAUSTFLOAT(4e+01f), FAUSTFLOAT(0.1f));
		ui_interface->closeBox();
		ui_interface->closeBox();
		ui_interface->closeBox();
	}
	
	virtual void compute(int count, FAUSTFLOAT** RESTRICT inputs, FAUSTFLOAT** RESTRICT outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = std::pow(1e+01f, 0.05f * static_cast<float>(fVslider0));
		float fSlow1 = static_cast<float>(fVslider1);
		float fSlow2 = fConst1 * (fSlow1 / std::sqrt(std::max<float>(0.0f, fSlow0)));
		float fSlow3 = (1.0f - fSlow2) / (fSlow2 + 1.0f);
		float fSlow4 = std::cos(fConst1 * fSlow1) * (fSlow3 + 1.0f);
		float fSlow5 = std::pow(1e+01f, 0.05f * static_cast<float>(fVslider2));
		float fSlow6 = static_cast<float>(fVslider3);
		float fSlow7 = fConst1 * (fSlow6 / std::sqrt(std::max<float>(0.0f, fSlow5)));
		float fSlow8 = (1.0f - fSlow7) / (fSlow7 + 1.0f);
		float fSlow9 = std::cos(fConst1 * fSlow6) * (fSlow8 + 1.0f);
		float fSlow10 = static_cast<float>(fVslider4);
		float fSlow11 = std::exp(-(fConst3 / fSlow10));
		float fSlow12 = ChainDsp_faustpower2_f(fSlow11);
		float fSlow13 = 1.0f - fSlow12;
		float fSlow14 = std::cos(fConst1 * static_cast<float>(fVslider5));
		float fSlow15 = 1.0f - fSlow14 * fSlow12;
		float fSlow16 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow15) / ChainDsp_faustpower2_f(fSlow13) + -1.0f));
		float fSlow17 = fSlow15 / fSlow13;
		float fSlow18 = fSlow17 - fSlow16;
		float fSlow19 = 1.0f / std::tan(fConst4 * static_cast<float>(fVslider6));
		float fSlow20 = 1.0f - fSlow19;
		float fSlow21 = 1.0f / (fSlow19 + 1.0f);
		float fSlow22 = static_cast<float>(fVslider7);
		float fSlow23 = std::exp(-(fConst3 / fSlow22)) / fSlow11 + -1.0f;
		float fSlow24 = fSlow11 * (fSlow16 + (1.0f - fSlow17));
		int iSlow25 = static_cast<int>(static_cast<float>(fCheckbox0));
		float fSlow26 = std::max<float>(fConst7, 0.001f * static_cast<float>(fHslider0));
		float fSlow27 = 0.5f * fSlow26;
		int iSlow28 = std::fabs(fSlow27) < 1.1920929e-07f;
		float fSlow29 = ((iSlow28) ? 0.0f : std::exp(-(fConst7 / ((iSlow28) ? 1.0f : fSlow27))));
		float fSlow30 = 1.0f - fSlow29;
		float fSlow31 = static_cast<float>(fHslider1);
		int iSlow32 = static_cast<int>(static_cast<float>(fCheckbox1));
		float fSlow33 = 0.5f * ((static_cast<int>(static_cast<float>(fCheckbox3))) ? 2.0f : static_cast<float>(fHslider2));
		float fSlow34 = ((static_cast<int>(static_cast<float>(fCheckbox2))) ? -fSlow33 : fSlow33);
		float fSlow35 = fConst1 * static_cast<float>(fHslider3);
		float fSlow36 = std::cos(fSlow35);
		float fSlow37 = std::sin(fSlow35);
		float fSlow38 = static_cast<float>(fHslider4);
		float fSlow39 = 3.1415927f * (fSlow38 - std::max<float>(fSlow38, static_cast<float>(fHslider5)));
		float fSlow40 = 6.2831855f * fSlow38;
		float fSlow41 = static_cast<float>(fHslider6);
		float fSlow42 = fConst7 * ChainDsp_faustpower4_f(fSlow41);
		float fSlow43 = fConst7 * ChainDsp_faustpower3_f(fSlow41);
		float fSlow44 = fConst7 * ChainDsp_faustpower2_f(fSlow41);
		float fSlow45 = fConst7 * fSlow41;
		float fSlow46 = std::exp(-(fConst4 * static_cast<float>(fHslider7)));
		float fSlow47 = ChainDsp_faustpower2_f(fSlow46);
		float fSlow48 = 2.0f * fSlow46;
		float fSlow49 = static_cast<float>(fHslider8);
		int iSlow50 = static_cast<int>(static_cast<float>(fCheckbox4));
		float fSlow51 = 0.001f * static_cast<float>(fHslider9);
		int iSlow52 = static_cast<int>(static_cast<float>(fCheckbox5));
		float fSlow53 = fConst8 * static_cast<float>(fHslider10);
		float fSlow54 = fConst8 * static_cast<float>(fHslider11);
		float fSlow55 = std::pow(1e+01f, 0.05f * static_cast<float>(fHslider12));
		float fSlow56 = 1.0f - fSlow33;
		float fSlow57 = std::max<float>(fConst7, 0.001f * static_cast<float>(fHslider13));
		int iSlow58 = std::fabs(fSlow57) < 1.1920929e-07f;
		float fSlow59 = ((iSlow58) ? 0.0f : std::exp(-(fConst7 / ((iSlow58) ? 1.0f : fSlow57))));
		int iSlow60 = std::fabs(fSlow26) < 1.1920929e-07f;
		float fSlow61 = ((iSlow60) ? 0.0f : std::exp(-(fConst7 / ((iSlow60) ? 1.0f : fSlow26))));
		float fSlow62 = 1.0f / std::max<float>(1.1920929e-07f, static_cast<float>(fHslider14)) + -1.0f;
		float fSlow63 = std::pow(1e+01f, 0.05f * static_cast<float>(fHslider15));
		int iSlow64 = static_cast<int>(std::min<float>(8192.0f, std::max<float>(0.0f, fConst10 * static_cast<float>(fVslider8))));
		float fSlow65 = std::exp(-(fConst13 / fSlow10));
		float fSlow66 = ChainDsp_faustpower2_f(fSlow65);
		float fSlow67 = 1.0f - fSlow66;
		float fSlow68 = 1.0f - fSlow66 * fSlow14;
		float fSlow69 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow68) / ChainDsp_faustpower2_f(fSlow67) + -1.0f));
		float fSlow70 = fSlow68 / fSlow67;
		float fSlow71 = fSlow70 - fSlow69;
		float fSlow72 = std::exp(-(fConst13 / fSlow22)) / fSlow65 + -1.0f;
		float fSlow73 = fSlow65 * (fSlow69 + (1.0f - fSlow70));
		float fSlow74 = std::exp(-(fConst18 / fSlow10));
		float fSlow75 = ChainDsp_faustpower2_f(fSlow74);
		float fSlow76 = 1.0f - fSlow75;
		float fSlow77 = 1.0f - fSlow14 * fSlow75;
		float fSlow78 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow77) / ChainDsp_faustpower2_f(fSlow76) + -1.0f));
		float fSlow79 = fSlow77 / fSlow76;
		float fSlow80 = fSlow79 - fSlow78;
		float fSlow81 = std::exp(-(fConst18 / fSlow22)) / fSlow74 + -1.0f;
		float fSlow82 = fSlow74 * (fSlow78 + (1.0f - fSlow79));
		float fSlow83 = std::exp(-(fConst23 / fSlow10));
		float fSlow84 = ChainDsp_faustpower2_f(fSlow83);
		float fSlow85 = 1.0f - fSlow84;
		float fSlow86 = 1.0f - fSlow14 * fSlow84;
		float fSlow87 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow86) / ChainDsp_faustpower2_f(fSlow85) + -1.0f));
		float fSlow88 = fSlow86 / fSlow85;
		float fSlow89 = fSlow88 - fSlow87;
		float fSlow90 = std::exp(-(fConst23 / fSlow22)) / fSlow83 + -1.0f;
		float fSlow91 = fSlow83 * (fSlow87 + (1.0f - fSlow88));
		float fSlow92 = std::exp(-(fConst28 / fSlow10));
		float fSlow93 = ChainDsp_faustpower2_f(fSlow92);
		float fSlow94 = 1.0f - fSlow93;
		float fSlow95 = 1.0f - fSlow14 * fSlow93;
		float fSlow96 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow95) / ChainDsp_faustpower2_f(fSlow94) + -1.0f));
		float fSlow97 = fSlow95 / fSlow94;
		float fSlow98 = fSlow97 - fSlow96;
		float fSlow99 = std::exp(-(fConst28 / fSlow22)) / fSlow92 + -1.0f;
		float fSlow100 = fSlow92 * (fSlow96 + (1.0f - fSlow97));
		float fSlow101 = std::exp(-(fConst33 / fSlow10));
		float fSlow102 = ChainDsp_faustpower2_f(fSlow101);
		float fSlow103 = 1.0f - fSlow102;
		float fSlow104 = 1.0f - fSlow14 * fSlow102;
		float fSlow105 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow104) / ChainDsp_faustpower2_f(fSlow103) + -1.0f));
		float fSlow106 = fSlow104 / fSlow103;
		float fSlow107 = fSlow106 - fSlow105;
		float fSlow108 = std::exp(-(fConst33 / fSlow22)) / fSlow101 + -1.0f;
		float fSlow109 = fSlow101 * (fSlow105 + (1.0f - fSlow106));
		float fSlow110 = std::exp(-(fConst38 / fSlow10));
		float fSlow111 = ChainDsp_faustpower2_f(fSlow110);
		float fSlow112 = 1.0f - fSlow111;
		float fSlow113 = 1.0f - fSlow14 * fSlow111;
		float fSlow114 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow113) / ChainDsp_faustpower2_f(fSlow112) + -1.0f));
		float fSlow115 = fSlow113 / fSlow112;
		float fSlow116 = fSlow115 - fSlow114;
		float fSlow117 = std::exp(-(fConst38 / fSlow22)) / fSlow110 + -1.0f;
		float fSlow118 = fSlow110 * (fSlow114 + (1.0f - fSlow115));
		float fSlow119 = std::exp(-(fConst43 / fSlow10));
		float fSlow120 = ChainDsp_faustpower2_f(fSlow119);
		float fSlow121 = 1.0f - fSlow120;
		float fSlow122 = 1.0f - fSlow14 * fSlow120;
		float fSlow123 = std::sqrt(std::max<float>(0.0f, ChainDsp_faustpower2_f(fSlow122) / ChainDsp_faustpower2_f(fSlow121) + -1.0f));
		float fSlow124 = fSlow122 / fSlow121;
		float fSlow125 = fSlow124 - fSlow123;
		float fSlow126 = std::exp(-(fConst43 / fSlow22)) / fSlow119 + -1.0f;
		float fSlow127 = fSlow119 * (fSlow123 + (1.0f - fSlow124));
		float fSlow128 = fConst8 * static_cast<float>(fVslider9);
		float fSlow129 = fConst8 * std::pow(1e+01f, 0.05f * static_cast<float>(fVslider10));
		for (int i0 = 0; i0 < count; i0 = i0 + 1) {
			iVec0[0] = 1;
			float fTemp0 = fSlow4 * fRec0[1];
			float fTemp1 = fSlow9 * fRec1[1];
			fRec13[0] = -(fSlow21 * (fSlow20 * fRec13[1] - (fRec6[1] + fRec6[2])));
			fRec12[0] = fSlow24 * (fRec6[1] + fSlow23 * fRec13[0]) + fSlow18 * fRec12[1];
			fVec1[IOTA0 & 16383] = 0.35355338f * fRec12[0] + 1e-20f;
			fRec17[0] = fSlow37 * fRec18[1] + fSlow36 * fRec17[1];
			fRec18[0] = static_cast<float>(1 - iVec0[1]) + fSlow36 * fRec18[1] - fSlow37 * fRec17[1];
			float fTemp2 = fSlow40 - fSlow39 * (1.0f - fRec18[0]);
			float fTemp3 = fRec19[1] * std::cos(fSlow42 * fTemp2);
			float fTemp4 = fRec20[1] * std::cos(fSlow43 * fTemp2);
			float fTemp5 = fRec21[1] * std::cos(fSlow44 * fTemp2);
			float fTemp6 = fRec22[1] * std::cos(fSlow45 * fTemp2);
			fRec24[0] = fSlow51 + 0.999f * fRec24[1];
			float fTemp7 = fConst1 * fRec24[0];
			float fTemp8 = 1.0f - fTemp7;
			float fTemp9 = static_cast<float>(input0[i0]);
			fRec30[0] = fSlow53 + fConst9 * fRec30[1];
			fRec31[0] = fSlow54 + fConst9 * fRec31[1];
			float fTemp10 = std::max<float>(-1.0f, std::min<float>(1.0f, fRec31[0] + std::pow(1e+01f, 2.0f * fRec30[0]) * ((iSlow52) ? 0.0f : fTemp9)));
			float fTemp11 = fTemp10 * (1.0f - 0.33333334f * ChainDsp_faustpower2_f(fTemp10));
			fVec2[0] = fTemp11;
			fRec29[0] = 0.995f * fRec29[1] + fTemp11 - fVec2[1];
			float fTemp12 = ((iSlow52) ? fTemp9 : fRec29[0]);
			fRec28[0] = ((iSlow50) ? 0.0f : fTemp12) + fTemp8 * fRec28[1] - 3.2f * fRec23[1];
			fRec27[0] = fRec28[0] + fTemp8 * fRec27[1];
			fRec26[0] = fRec27[0] + fTemp8 * fRec26[1];
			fRec25[0] = fRec26[0] + fRec25[1] * fTemp8;
			fRec23[0] = fRec25[0] * ChainDsp_faustpower4_f(fTemp7);
			float fTemp13 = ((iSlow50) ? fTemp12 : 4.0f * fRec23[0]);
			float fTemp14 = ((iSlow32) ? 0.0f : fTemp13);
			float fTemp15 = fSlow55 * fTemp14;
			fRec22[0] = fTemp15 + fSlow49 * fRec16[1] + fSlow48 * fTemp6 - fSlow47 * fRec22[2];
			fRec21[0] = fRec22[2] + fSlow47 * (fRec22[0] - fRec21[2]) - fSlow48 * (fTemp6 - fTemp5);
			fRec20[0] = fRec21[2] + fSlow47 * (fRec21[0] - fRec20[2]) - fSlow48 * (fTemp5 - fTemp4);
			fRec19[0] = fRec20[2] + fSlow47 * (fRec20[0] - fRec19[2]) - fSlow48 * (fTemp4 - fTemp3);
			fRec16[0] = fRec19[2] + fSlow47 * fRec19[0] - fSlow48 * fTemp3;
			float fTemp16 = fSlow55 * fTemp14 * fSlow56;
			float fTemp17 = ((iSlow32) ? fTemp13 : fTemp16 + fRec16[0] * fSlow34);
			float fTemp18 = ((iSlow25) ? 0.0f : fTemp17);
			float fTemp19 = fSlow40 - fSlow39 * (1.0f - fRec17[0]);
			float fTemp20 = fRec33[1] * std::cos(fSlow42 * fTemp19);
			float fTemp21 = fRec34[1] * std::cos(fSlow43 * fTemp19);
			float fTemp22 = fRec35[1] * std::cos(fSlow44 * fTemp19);
			float fTemp23 = fRec36[1] * std::cos(fSlow45 * fTemp19);
			fRec36[0] = fTemp15 + fSlow49 * fRec32[1] + fSlow48 * fTemp23 - fSlow47 * fRec36[2];
			fRec35[0] = fRec36[2] + fSlow47 * (fRec36[0] - fRec35[2]) - fSlow48 * (fTemp23 - fTemp22);
			fRec34[0] = fRec35[2] + fSlow47 * (fRec35[0] - fRec34[2]) - fSlow48 * (fTemp22 - fTemp21);
			fRec33[0] = fRec34[2] + fSlow47 * (fRec34[0] - fRec33[2]) - fSlow48 * (fTemp21 - fTemp20);
			fRec32[0] = fRec33[2] + fSlow47 * fRec33[0] - fSlow48 * fTemp20;
			float fTemp24 = ((iSlow32) ? fTemp13 : fTemp16 + fRec32[0] * fSlow34);
			float fTemp25 = ((iSlow25) ? 0.0f : fTemp24);
			float fTemp26 = std::fabs(std::fabs(fTemp25) + std::fabs(fTemp18));
			float fTemp27 = ((fTemp26 > fRec15[1]) ? fSlow61 : fSlow59);
			fRec15[0] = fTemp26 * (1.0f - fTemp27) + fRec15[1] * fTemp27;
			fRec14[0] = fSlow62 * std::max<float>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec15[0])) - fSlow31, 0.0f) * fSlow30 + fSlow29 * fRec14[1];
			float fTemp28 = std::pow(1e+01f, 0.05f * fRec14[0]);
			float fTemp29 = fTemp25 * fTemp28;
			float fTemp30 = ((iSlow25) ? fTemp24 : fSlow63 * fTemp29);
			fVec3[IOTA0 & 16383] = fTemp30;
			float fTemp31 = 0.3f * fVec3[(IOTA0 - iSlow64) & 16383];
			float fTemp32 = fTemp31 + fVec1[(IOTA0 - iConst6) & 16383] - 0.6f * fRec10[1];
			fVec4[IOTA0 & 4095] = fTemp32;
			fRec10[0] = fVec4[(IOTA0 - iConst11) & 4095];
			float fRec11 = 0.6f * fTemp32;
			fRec40[0] = -(fSlow21 * (fSlow20 * fRec40[1] - (fRec2[1] + fRec2[2])));
			fRec39[0] = fSlow73 * (fRec2[1] + fSlow72 * fRec40[0]) + fSlow71 * fRec39[1];
			fVec5[IOTA0 & 16383] = 0.35355338f * fRec39[0] + 1e-20f;
			float fTemp33 = fVec5[(IOTA0 - iConst15) & 16383] + fTemp31 - 0.6f * fRec37[1];
			fVec6[IOTA0 & 2047] = fTemp33;
			fRec37[0] = fVec6[(IOTA0 - iConst16) & 2047];
			float fRec38 = 0.6f * fTemp33;
			float fTemp34 = fRec38 + fRec11;
			fRec44[0] = -(fSlow21 * (fSlow20 * fRec44[1] - (fRec4[1] + fRec4[2])));
			fRec43[0] = fSlow82 * (fRec4[1] + fSlow81 * fRec44[0]) + fSlow80 * fRec43[1];
			fVec7[IOTA0 & 16383] = 0.35355338f * fRec43[0] + 1e-20f;
			float fTemp35 = fVec7[(IOTA0 - iConst20) & 16383] - (fTemp31 + 0.6f * fRec41[1]);
			fVec8[IOTA0 & 4095] = fTemp35;
			fRec41[0] = fVec8[(IOTA0 - iConst21) & 4095];
			float fRec42 = 0.6f * fTemp35;
			fRec48[0] = -(fSlow21 * (fSlow20 * fRec48[1] - (fRec8[1] + fRec8[2])));
			fRec47[0] = fSlow91 * (fRec8[1] + fSlow90 * fRec48[0]) + fSlow89 * fRec47[1];
			fVec9[IOTA0 & 16383] = 0.35355338f * fRec47[0] + 1e-20f;
			float fTemp36 = fVec9[(IOTA0 - iConst25) & 16383] - (fTemp31 + 0.6f * fRec45[1]);
			fVec10[IOTA0 & 2047] = fTemp36;
			fRec45[0] = fVec10[(IOTA0 - iConst26) & 2047];
			float fRec46 = 0.6f * fTemp36;
			float fTemp37 = fRec46 + fRec42 + fTemp34;
			fRec52[0] = -(fSlow21 * (fSlow20 * fRec52[1] - (fRec3[1] + fRec3[2])));
			fRec51[0] = fSlow100 * (fRec3[1] + fSlow99 * fRec52[0]) + fSlow98 * fRec51[1];
			fVec11[IOTA0 & 32767] = 0.35355338f * fRec51[0] + 1e-20f;
			float fTemp38 = fTemp18 * fTemp28;
			float fTemp39 = std::fabs(std::fabs(fTemp29) + std::fabs(fTemp38));
			float fTemp40 = ((fTemp39 > fRec54[1]) ? fSlow61 : fSlow59);
			fRec54[0] = fTemp39 * (1.0f - fTemp40) + fRec54[1] * fTemp40;
			fRec53[0] = fSlow62 * std::max<float>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, fRec54[0])) - fSlow31, 0.0f) * fSlow30 + fSlow29 * fRec53[1];
			fHbargraph0 = static_cast<FAUSTFLOAT>(2e+01f * std::log10(std::max<float>(1.1754944e-38f, std::pow(1e+01f, 0.05f * fRec53[0]))));
			float fTemp41 = ((iSlow25) ? fTemp17 : fSlow63 * fTemp38);
			fVec12[IOTA0 & 16383] = fTemp41;
			float fTemp42 = 0.3f * fVec12[(IOTA0 - iSlow64) & 16383];
			float fTemp43 = fTemp42 + 0.6f * fRec49[1] + fVec11[(IOTA0 - iConst30) & 32767];
			fVec13[IOTA0 & 4095] = fTemp43;
			fRec49[0] = fVec13[(IOTA0 - iConst31) & 4095];
			float fRec50 = -(0.6f * fTemp43);
			fRec58[0] = -(fSlow21 * (fSlow20 * fRec58[1] - (fRec7[1] + fRec7[2])));
			fRec57[0] = fSlow109 * (fRec7[1] + fSlow108 * fRec58[0]) + fSlow107 * fRec57[1];
			fVec14[IOTA0 & 16383] = 0.35355338f * fRec57[0] + 1e-20f;
			float fTemp44 = fVec14[(IOTA0 - iConst35) & 16383] + fTemp42 + 0.6f * fRec55[1];
			fVec15[IOTA0 & 4095] = fTemp44;
			fRec55[0] = fVec15[(IOTA0 - iConst36) & 4095];
			float fRec56 = -(0.6f * fTemp44);
			fRec62[0] = -(fSlow21 * (fSlow20 * fRec62[1] - (fRec5[1] + fRec5[2])));
			fRec61[0] = fSlow118 * (fRec5[1] + fSlow117 * fRec62[0]) + fSlow116 * fRec61[1];
			fVec16[IOTA0 & 32767] = 0.35355338f * fRec61[0] + 1e-20f;
			float fTemp45 = 0.6f * fRec59[1] + fVec16[(IOTA0 - iConst40) & 32767];
			fVec17[IOTA0 & 4095] = fTemp45 - fTemp42;
			fRec59[0] = fVec17[(IOTA0 - iConst41) & 4095];
			float fRec60 = 0.6f * (fTemp42 - fTemp45);
			fRec66[0] = -(fSlow21 * (fSlow20 * fRec66[1] - (fRec9[1] + fRec9[2])));
			fRec65[0] = fSlow127 * (fRec9[1] + fSlow126 * fRec66[0]) + fSlow125 * fRec65[1];
			fVec18[IOTA0 & 32767] = 0.35355338f * fRec65[0] + 1e-20f;
			float fTemp46 = 0.6f * fRec63[1] + fVec18[(IOTA0 - iConst45) & 32767];
			fVec19[IOTA0 & 2047] = fTemp46 - fTemp42;
			fRec63[0] = fVec19[(IOTA0 - iConst46) & 2047];
			float fRec64 = 0.6f * (fTemp42 - fTemp46);
			fRec2[0] = fRec63[1] + fRec59[1] + fRec55[1] + fRec49[1] + fRec45[1] + fRec41[1] + fRec10[1] + fRec37[1] + fRec64 + fRec60 + fRec56 + fRec50 + fTemp37;
			fRec3[0] = fRec45[1] + fRec41[1] + fRec10[1] + fRec37[1] + fTemp37 - (fRec63[1] + fRec59[1] + fRec55[1] + fRec49[1] + fRec64 + fRec60 + fRec50 + fRec56);
			float fTemp47 = fRec42 + fRec46;
			fRec4[0] = fRec55[1] + fRec49[1] + fRec10[1] + fRec37[1] + fRec56 + fRec50 + fTemp34 - (fRec63[1] + fRec59[1] + fRec45[1] + fRec41[1] + fRec64 + fRec60 + fTemp47);
			fRec5[0] = fRec63[1] + fRec59[1] + fRec10[1] + fRec37[1] + fRec64 + fRec60 + fTemp34 - (fRec55[1] + fRec49[1] + fRec45[1] + fRec41[1] + fRec56 + fRec50 + fTemp47);
			float fTemp48 = fRec11 + fRec46;
			float fTemp49 = fRec38 + fRec42;
			fRec6[0] = fRec59[1] + fRec49[1] + fRec41[1] + fRec37[1] + fRec60 + fRec50 + fTemp49 - (fRec63[1] + fRec55[1] + fRec45[1] + fRec10[1] + fRec64 + fRec56 + fTemp48);
			fRec7[0] = fRec63[1] + fRec55[1] + fRec41[1] + fRec37[1] + fRec64 + fRec56 + fTemp49 - (fRec59[1] + fRec49[1] + fRec45[1] + fRec10[1] + fRec60 + fRec50 + fTemp48);
			float fTemp50 = fRec11 + fRec42;
			float fTemp51 = fRec38 + fRec46;
			fRec8[0] = fRec63[1] + fRec49[1] + fRec45[1] + fRec37[1] + fRec64 + fRec50 + fTemp51 - (fRec59[1] + fRec55[1] + fRec41[1] + fRec10[1] + fRec60 + fRec56 + fTemp50);
			fRec9[0] = fRec59[1] + fRec55[1] + fRec45[1] + fRec37[1] + fRec60 + fRec56 + fTemp51 - (fRec63[1] + fRec49[1] + fRec41[1] + fRec10[1] + fRec64 + fRec50 + fTemp50);
			float fTemp52 = 0.37f * (fRec3[0] + fRec4[0]);
			float fTemp53 = fTemp52 + fTemp1;
			fRec1[0] = fTemp53 - fSlow8 * fRec1[2];
			float fTemp54 = fSlow8 * fRec1[0];
			float fTemp55 = 0.5f * (fTemp54 + fTemp52 + fRec1[2] - fTemp1 + fSlow5 * (fRec1[2] + fTemp54 - fTemp53));
			float fTemp56 = fTemp55 + fTemp0;
			fRec0[0] = fTemp56 - fSlow3 * fRec0[2];
			float fTemp57 = fSlow3 * fRec0[0];
			fRec67[0] = fSlow128 + fConst9 * fRec67[1];
			float fTemp58 = fRec67[0] + 1.0f;
			float fTemp59 = 1.0f - 0.5f * fTemp58;
			fRec68[0] = fSlow129 + fConst9 * fRec68[1];
			output0[i0] = static_cast<FAUSTFLOAT>(0.5f * fRec68[0] * (fTemp58 * fTemp30 + fTemp59 * (fTemp57 + fTemp55 + fRec0[2] - fTemp0 + fSlow0 * (fRec0[2] + fTemp57 - fTemp56))));
			float fTemp60 = fSlow4 * fRec69[1];
			float fTemp61 = fSlow9 * fRec70[1];
			float fTemp62 = 0.37f * (fRec3[0] - fRec4[0]);
			float fTemp63 = fTemp62 + fTemp61;
			fRec70[0] = fTemp63 - fSlow8 * fRec70[2];
			float fTemp64 = fSlow8 * fRec70[0];
			float fTemp65 = 0.5f * (fTemp64 + fTemp62 + fRec70[2] - fTemp61 + fSlow5 * (fRec70[2] + fTemp64 - fTemp63));
			float fTemp66 = fTemp65 + fTemp60;
			fRec69[0] = fTemp66 - fSlow3 * fRec69[2];
			float fTemp67 = fSlow3 * fRec69[0];
			output1[i0] = static_cast<FAUSTFLOAT>(0.5f * fRec68[0] * (fTemp58 * fTemp41 + fTemp59 * (fTemp67 + fTemp65 + fRec69[2] - fTemp60 + fSlow0 * (fRec69[2] + fTemp67 - fTemp66))));
			iVec0[1] = iVec0[0];
			fRec13[1] = fRec13[0];
			fRec12[1] = fRec12[0];
			IOTA0 = IOTA0 + 1;
			fRec17[1] = fRec17[0];
			fRec18[1] = fRec18[0];
			fRec24[1] = fRec24[0];
			fRec30[1] = fRec30[0];
			fRec31[1] = fRec31[0];
			fVec2[1] = fVec2[0];
			fRec29[1] = fRec29[0];
			fRec28[1] = fRec28[0];
			fRec27[1] = fRec27[0];
			fRec26[1] = fRec26[0];
			fRec25[1] = fRec25[0];
			fRec23[1] = fRec23[0];
			fRec22[2] = fRec22[1];
			fRec22[1] = fRec22[0];
			fRec21[2] = fRec21[1];
			fRec21[1] = fRec21[0];
			fRec20[2] = fRec20[1];
			fRec20[1] = fRec20[0];
			fRec19[2] = fRec19[1];
			fRec19[1] = fRec19[0];
			fRec16[1] = fRec16[0];
			fRec36[2] = fRec36[1];
			fRec36[1] = fRec36[0];
			fRec35[2] = fRec35[1];
			fRec35[1] = fRec35[0];
			fRec34[2] = fRec34[1];
			fRec34[1] = fRec34[0];
			fRec33[2] = fRec33[1];
			fRec33[1] = fRec33[0];
			fRec32[1] = fRec32[0];
			fRec15[1] = fRec15[0];
			fRec14[1] = fRec14[0];
			fRec10[1] = fRec10[0];
			fRec40[1] = fRec40[0];
			fRec39[1] = fRec39[0];
			fRec37[1] = fRec37[0];
			fRec44[1] = fRec44[0];
			fRec43[1] = fRec43[0];
			fRec41[1] = fRec41[0];
			fRec48[1] = fRec48[0];
			fRec47[1] = fRec47[0];
			fRec45[1] = fRec45[0];
			fRec52[1] = fRec52[0];
			fRec51[1] = fRec51[0];
			fRec54[1] = fRec54[0];
			fRec53[1] = fRec53[0];
			fRec49[1] = fRec49[0];
			fRec58[1] = fRec58[0];
			fRec57[1] = fRec57[0];
			fRec55[1] = fRec55[0];
			fRec62[1] = fRec62[0];
			fRec61[1] = fRec61[0];
			fRec59[1] = fRec59[0];
			fRec66[1] = fRec66[0];
			fRec65[1] = fRec65[0];
			fRec63[1] = fRec63[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec67[1] = fRec67[0];
			fRec68[1] = fRec68[0];
			fRec70[2] = fRec70[1];
			fRec70[1] = fRec70[0];
			fRec69[2] = fRec69[1];
			fRec69[1] = fRec69[0];
		}
	}

};

#endif
