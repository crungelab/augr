#pragma once

#include <RtAudio.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cctype>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace augr {

struct AudioConfig {
    bool enableInput = false;

    unsigned int inputDeviceId = 0;
    unsigned int outputDeviceId = 0;

    unsigned int inputChannels = 0;
    unsigned int outputChannels = 0;

    unsigned int sampleRate = 48000;
    unsigned int frames = 512;

    bool nonInterleaved = true;
};

class AudioConfigurator {
public:
    explicit AudioConfigurator(RtAudio &audio) : audio_(audio) {}

    bool configure(AudioConfig &config) {
        const auto deviceCount = audio_.getDeviceCount();
        spdlog::debug("AudioConfigurator: device count={}", deviceCount);

        if (deviceCount < 1) {
            spdlog::error("AudioConfigurator: no audio devices found");
            return false;
        }

        if (config.enableInput) {
            spdlog::debug("AudioConfigurator: selecting duplex device for "
                          "requested sample rate {}",
                          config.sampleRate);

            auto duplexId = selectBestDevice(
                /*needInput=*/true,
                /*needOutput=*/true, config.sampleRate);

            if (!duplexId.has_value()) {
                spdlog::error(
                    "AudioConfigurator: failed to find suitable duplex device");
                return false;
            }

            auto infoOpt = getDeviceInfoSafe(*duplexId);
            if (!infoOpt.has_value()) {
                spdlog::error("AudioConfigurator: selected duplex device {} "
                              "could not be queried",
                              *duplexId);
                return false;
            }

            const RtAudio::DeviceInfo &info = *infoOpt;

            config.inputDeviceId = *duplexId;
            config.outputDeviceId = *duplexId;

            config.inputChannels =
                std::min<unsigned int>(2, info.inputChannels);
            config.outputChannels =
                std::min<unsigned int>(2, info.outputChannels);
            config.sampleRate = chooseSampleRate(info, config.sampleRate);

            if (config.frames == 0) {
                config.frames = 512;
            }

            spdlog::info(
                "AudioConfigurator: selected duplex device id={} name='{}' "
                "inputChannels={} outputChannels={} sampleRate={} frames={}",
                *duplexId, info.name, config.inputChannels,
                config.outputChannels, config.sampleRate, config.frames);

            return config.inputChannels > 0 && config.outputChannels > 0;
        }

        spdlog::debug("AudioConfigurator: selecting output device for "
                      "requested sample rate {}",
                      config.sampleRate);

        auto outputId = selectBestDevice(
            /*needInput=*/false,
            /*needOutput=*/true, config.sampleRate);

        if (!outputId.has_value()) {
            spdlog::error(
                "AudioConfigurator: failed to find suitable output device");
            return false;
        }

        auto infoOpt = getDeviceInfoSafe(*outputId);
        if (!infoOpt.has_value()) {
            spdlog::error("AudioConfigurator: selected output device {} could "
                          "not be queried",
                          *outputId);
            return false;
        }

        const RtAudio::DeviceInfo &info = *infoOpt;

        config.outputDeviceId = *outputId;
        config.outputChannels = std::min<unsigned int>(2, info.outputChannels);

        config.inputDeviceId = 0;
        config.inputChannels = 0;

        config.sampleRate = chooseSampleRate(info, config.sampleRate);

        if (config.frames == 0) {
            config.frames = 512;
        }

        spdlog::info(
            "AudioConfigurator: selected output device id={} name='{}' "
            "outputChannels={} sampleRate={} frames={}",
            *outputId, info.name, config.outputChannels, config.sampleRate,
            config.frames);

        return config.outputChannels > 0;
    }

private:
    RtAudio &audio_;

    static std::string toLower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });
        return s;
    }

    static bool contains(const std::string &haystack,
                         const std::string &needle) {
        return haystack.find(needle) != std::string::npos;
    }

    static bool supportsSampleRate(const RtAudio::DeviceInfo &info,
                                   unsigned int sampleRate) {
        return std::find(info.sampleRates.begin(), info.sampleRates.end(),
                         sampleRate) != info.sampleRates.end();
    }

    std::optional<RtAudio::DeviceInfo>
    getDeviceInfoSafe(unsigned int id) const {
        try {
            return audio_.getDeviceInfo(id);
        } catch (const std::exception &e) {
            spdlog::warn("AudioConfigurator: getDeviceInfo({}) failed: {}", id,
                         e.what());
            return std::nullopt;
        } catch (...) {
            spdlog::warn("AudioConfigurator: getDeviceInfo({}) failed with "
                         "unknown exception",
                         id);
            return std::nullopt;
        }
    }

    unsigned int chooseSampleRate(const RtAudio::DeviceInfo &info,
                                  unsigned int preferred) const {
        if (preferred != 0 && supportsSampleRate(info, preferred)) {
            spdlog::debug("AudioConfigurator: device '{}' supports requested "
                          "sample rate {}",
                          info.name, preferred);
            return preferred;
        }

        if (info.preferredSampleRate != 0 &&
            supportsSampleRate(info, info.preferredSampleRate)) {
            spdlog::debug(
                "AudioConfigurator: device '{}' using preferred sample rate {}",
                info.name, info.preferredSampleRate);
            return info.preferredSampleRate;
        }

        if (!info.sampleRates.empty()) {
            if (supportsSampleRate(info, 48000)) {
                spdlog::debug("AudioConfigurator: device '{}' falling back to "
                              "sample rate 48000",
                              info.name);
                return 48000;
            }

            if (supportsSampleRate(info, 44100)) {
                spdlog::debug("AudioConfigurator: device '{}' falling back to "
                              "sample rate 44100",
                              info.name);
                return 44100;
            }

            spdlog::debug("AudioConfigurator: device '{}' falling back to "
                          "first supported sample rate {}",
                          info.name, info.sampleRates.front());
            return info.sampleRates.front();
        }

        const auto fallback = preferred != 0 ? preferred : 48000;
        spdlog::warn("AudioConfigurator: device '{}' reported no sample rates, "
                     "using fallback {}",
                     info.name, fallback);
        return fallback;
    }

    int scoreDevice(const RtAudio::DeviceInfo &info, bool needInput,
                    bool needOutput, unsigned int desiredSampleRate) const {
        if (needInput && info.inputChannels == 0) {
            return std::numeric_limits<int>::min();
        }

        if (needOutput && info.outputChannels == 0) {
            return std::numeric_limits<int>::min();
        }

        int score = 0;
        const std::string name = toLower(info.name);

        if (contains(name, "default")) {
            score -= 100;
        }
        if (contains(name, "pulse")) {
            score -= 100;
        }
        if (contains(name, "sysdefault")) {
            score -= 80;
        }
        if (contains(name, "dmix")) {
            score -= 80;
        }
        if (contains(name, "dsnoop")) {
            score -= 80;
        }
        if (contains(name, "hdmi")) {
            score -= 40;
        }

        if (contains(name, "analog")) {
            score += 40;
        }
        if (contains(name, "alc")) {
            score += 20;
        }
        if (contains(name, "intel")) {
            score += 10;
        }
        if (contains(name, "pch")) {
            score += 10;
        }

        if (supportsSampleRate(info, desiredSampleRate)) {
            score += 50;
        }
        if (info.preferredSampleRate == desiredSampleRate) {
            score += 25;
        }

        if (needInput && needOutput) {
            if (info.inputChannels >= 2 && info.outputChannels >= 2) {
                score += 20;
            }
            if (info.inputChannels == 2) {
                score += 10;
            }
            if (info.outputChannels == 2) {
                score += 10;
            }

            if (info.inputChannels > 8) {
                score -= 20;
            }
            if (info.outputChannels > 8) {
                score -= 20;
            }
        } else if (needOutput) {
            if (info.outputChannels >= 2) {
                score += 20;
            }
            if (info.outputChannels == 2) {
                score += 10;
            }
            if (info.outputChannels > 8) {
                score -= 20;
            }
        } else if (needInput) {
            if (info.inputChannels >= 2) {
                score += 20;
            }
            if (info.inputChannels == 2) {
                score += 10;
            }
            if (info.inputChannels > 8) {
                score -= 20;
            }
        }

        if (needInput && info.isDefaultInput) {
            score += 5;
        }
        if (needOutput && info.isDefaultOutput) {
            score += 5;
        }

        return score;
    }

    std::optional<unsigned int>
    selectBestDevice(bool needInput, bool needOutput,
                     unsigned int desiredSampleRate) const {
        //const unsigned int count = audio_.getDeviceCount();
        const auto deviceIds = audio_.getDeviceIds();
        int bestScore = std::numeric_limits<int>::min();
        std::optional<unsigned int> bestId;

        for (auto id : deviceIds) {
            auto infoOpt = getDeviceInfoSafe(id);
            if (!infoOpt.has_value()) {
                continue;
            }

            const RtAudio::DeviceInfo &info = *infoOpt;
            const int score =
                scoreDevice(info, needInput, needOutput, desiredSampleRate);

            spdlog::debug("AudioConfigurator: candidate id={} name='{}' in={} "
                          "out={} preferredRate={} score={}",
                          id, info.name, info.inputChannels,
                          info.outputChannels, info.preferredSampleRate, score);

            if (score > bestScore) {
                bestScore = score;
                bestId = id;
            }
        }

        if (bestId.has_value()) {
            spdlog::debug("AudioConfigurator: best device id={} score={}",
                          *bestId, bestScore);
        } else {
            spdlog::warn("AudioConfigurator: no suitable device found");
        }

        return bestId;
    }
};

} // namespace augr