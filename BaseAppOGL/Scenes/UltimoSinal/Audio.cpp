#include "Audio.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>

#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <mutex>
#include <thread>

#pragma comment(lib, "winmm.lib")

namespace ultimo {
namespace {
constexpr unsigned SampleRate = 22050;
constexpr unsigned Frames = 512;
constexpr unsigned BufferCount = 3;
constexpr float Pi = 3.14159265358979323846f;
constexpr float SampleTime = 1.f / SampleRate;

float Unit(float value) {
    return std::isfinite(value) ? (value < 0.f ? 0.f : (value > 1.f ? 1.f : value)) : 0.f;
}

float Pan(float value) {
    return std::isfinite(value) ? (value < -1.f ? -1.f : (value > 1.f ? 1.f : value)) : 0.f;
}

float Distance(float value) {
    return std::isfinite(value) && value >= 0.f ? (value > 1000.f ? 1000.f : value) : 1000.f;
}

float DistanceGain(float distance) {
    distance = Distance(distance);
    if (distance >= 24.f) return 0.f;
    const float offset = (distance > 1.f ? distance - 1.f : 0.f) / 4.f;
    // A soft end to the audible radius avoids a discontinuity at 24 m.
    const float fade = distance > 18.f ? (24.f - distance) / 6.f : 1.f;
    return fade / (1.f + offset * offset);
}

float CueLength(int kind) {
    constexpr float lengths[] = { .18f, .55f, .86f, .64f, .28f, LensPlaybackDuration };
    if (kind >= 0 && kind <= 5) return lengths[kind];
    return kind == 10 ? .20f : (kind == 11 ? 1.10f : 0.f);
}

float Noise(std::uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return (static_cast<float>(state & 0x00ffffffu) / 8388607.5f) - 1.f;
}

// A short fade on both ends avoids clicks even when an effect is interrupted.
float Envelope(float time, float length, float attack = .008f) {
    if (time < 0.f || time >= length) return 0.f;
    const float fadeIn = Unit(time / attack);
    const float fadeOut = Unit((length - time) / .035f);
    return fadeIn * fadeOut;
}

struct Command { int kind = 0; float pan = 0.f; unsigned generation = 0; };
struct Voice {
    bool active = false;
    int kind = 0;
    double time = 0.;
    float length = 0.f;
    float left = 0.f;
    float right = 0.f;
    float noiseLow = 0.f;
    float variation = 1.f;
    float surface = 0.f;
    float lastSample = 0.f;
};

float VoiceSample(Voice& voice, std::uint32_t& random) {
    const float t = static_cast<float>(voice.time);
    const float noise = Noise(random);
    voice.noiseLow += .14f * (noise - voice.noiseLow);
    float sample = 0.f;
    switch (voice.kind) {
    case 0: // Gentle wooden tap and a small confirmation tone.
        sample = .13f * std::sin(2.f * Pi * 760.f * t) * std::exp(-28.f * t)
            + .09f * voice.noiseLow * std::exp(-40.f * t);
        break;
    case 1: // A switch followed by a rising electrical hum.
        sample = .08f * noise * std::exp(-65.f * t)
            + .08f * std::sin(2.f * Pi * (110.f * t + 150.f * t * t))
                * std::sin(Pi * Unit(t / voice.length));
        break;
    case 2: { // Receiver tuning, not a new outgoing or encoded transmission.
        sample = .016f * voice.noiseLow * (1.f - Unit(t / .50f))
            + .035f * std::sin(2.f * Pi * (380.f * t + 90.f * t * t))
                * Envelope(t - .12f, .38f, .07f);
        break;
    }
    case 3: // Hinge friction and a low wooden knock.
        sample = .085f * voice.noiseLow * (1.f - Unit(t / voice.length))
            + .045f * std::sin(2.f * Pi * (160.f * t - 55.f * t * t))
                * std::sin(Pi * Unit(t / voice.length))
            + .08f * std::sin(2.f * Pi * 75.f * t) * std::exp(-25.f * t);
        break;
    case 4: // Soft double low note: no loud alarm.
        sample = .1f * std::sin(2.f * Pi * 220.f * t)
            * (Envelope(t, .10f) + Envelope(t - .15f, .10f));
        break;
    case 5: // Local lens replay: three long pulses, never repeated automatically.
        for (int pulse = 0; pulse < 3; ++pulse) {
            const float pulseTime = t - LensLeadIn - pulse * LensPulseSpacing;
            sample += .115f * std::sin(2.f * Pi * 540.f * pulseTime)
                * Envelope(pulseTime, LensPulseDuration, .022f);
        }
        break;
    case 10: { // Stone/gravel outside, a softer wooden impact inside.
        const float pitch = (78.f + voice.surface * 18.f) * voice.variation;
        sample = (.18f - voice.surface * .07f) * voice.noiseLow * std::exp(-24.f * t)
            + (.055f + voice.surface * .025f) * std::sin(2.f * Pi * pitch * t) * std::exp(-32.f * t);
        break;
    }
    case 11: // Infrequent structural creak, below the investigation cues.
        sample = .027f * std::sin(2.f * Pi * (52.f * t + 13.f * t * t) * voice.variation)
                * std::sin(Pi * Unit(t / voice.length))
            + .013f * voice.noiseLow * std::sin(Pi * Unit(t / voice.length));
        break;
    }
    sample *= Envelope(t, voice.length, .004f);
    voice.lastSample = sample;
    voice.time += SampleTime;
    if (voice.time >= voice.length) voice.active = false;
    return sample;
}

std::int16_t Pcm(float value) {
    // Soft limiting keeps simultaneous cues within the signed PCM range.
    const float limited = value / (1.f + std::fabs(value));
    return static_cast<std::int16_t>(limited * 30000.f);
}

struct MixParameters {
    bool indoors = false, walking = false, perspective = false, powered = false, playbackActive = true;
    float master = .65f, ambient = .65f, effects = .75f;
    float radioDistance = 1000.f, radioPan = 0.f, panelDistance = 1000.f, panelPan = 0.f;
    unsigned generation = 0;
};

// This mixer owns no device or synchronization objects. Its only clock is PCM
// sample time, so playback remains identical at different rendering frame rates.
struct Mixer {
    std::array<Voice, 16> voices{};
    Voice structure;
    std::uint32_t random = 0x5a17c3e9u;
    float seaL = 0.f, seaR = 0.f, windL = 0.f, windR = 0.f;
    double phase = 0.0;
    float insideMix = 0.f, masterMix = 0.f, ambientMix = 0.f, effectsMix = 0.f;
    float radioGain = 0.f, panelGain = 0.f, radioPan = 0.f, panelPan = 0.f, powerMix = 0.f;
    float playbackMix = 1.f;
    float stepClock = 0.f, structureClock = 19.f;
    bool previousWalking = false, leftStep = false;
    unsigned renderedGeneration = 0;

    void StartVoice(int kind, float pan) {
        if (CueLength(kind) == 0.f) return;
        Voice* slot = nullptr;
        for (auto& voice : voices) if (!voice.active) { slot = &voice; break; }
        // Bounded polyphony: discard an excess event, never allocate in the mixer.
        if (!slot) return;
        *slot = Voice{};
        slot->active = true;
        slot->kind = kind;
        slot->length = CueLength(kind);
        slot->left = std::sqrt(.5f * (1.f - Pan(pan)));
        slot->right = std::sqrt(.5f * (1.f + Pan(pan)));
        slot->variation = .94f + .12f * Unit(.5f + .5f * Noise(random));
        slot->surface = insideMix;
    }

    void Render(std::array<std::int16_t, Frames * 2>& samples, const MixParameters& p,
        const Command* pending = nullptr, unsigned count = 0) {
        if (renderedGeneration != p.generation || p.master == 0.f) {
            for (auto& voice : voices) voice.active = false;
            structure.active = false;
        }
        if (p.effects == 0.f) for (auto& voice : voices) voice.active = false;
        renderedGeneration = p.generation;
        if (p.master > 0.f && p.effects > 0.f)
            for (unsigned i = 0; i < count; ++i)
                if (pending[i].generation == p.generation) StartVoice(pending[i].kind, pending[i].pan);
        const bool isWalking = p.walking && p.playbackActive;
        if (isWalking && !previousWalking) stepClock = .36f;
        previousWalking = isWalking;
        if (!isWalking) stepClock = 0.f;
        const float targetInside = p.indoors ? 1.f : 0.f;
        const float targetRadio = p.perspective ? DistanceGain(p.radioDistance) : 0.f;
        const float targetPanel = p.perspective ? DistanceGain(p.panelDistance) : 0.f;
        const float targetPower = p.perspective && p.powered ? 1.f : 0.f;

        for (unsigned frame = 0; frame < Frames; ++frame) {
            masterMix += .002f * (p.master - masterMix);
            ambientMix += .001f * (p.ambient - ambientMix);
            effectsMix += .002f * (p.effects - effectsMix);
            insideMix += .00010f * (targetInside - insideMix);
            radioGain += .0006f * (targetRadio - radioGain);
            panelGain += .0006f * (targetPanel - panelGain);
            radioPan += .0006f * (p.radioPan - radioPan);
            panelPan += .0006f * (p.panelPan - panelPan);
            powerMix += .00012f * (targetPower - powerMix);
            playbackMix += .004f * ((p.playbackActive ? 1.f : 0.f) - playbackMix);
            if (isWalking) {
                stepClock += SampleTime;
                if (stepClock >= .47f) {
                    stepClock -= .47f;
                    leftStep = !leftStep;
                    if (p.master > 0.f && p.effects > 0.f)
                        StartVoice(10, leftStep ? -.24f : .24f);
                }
            }

            const float whiteL = Noise(random), whiteR = Noise(random);
            seaL += .006f * (whiteL - seaL);
            seaR += .006f * (whiteR - seaR);
            windL += .026f * (whiteL - windL);
            windR += .026f * (whiteR - windR);
            phase += SampleTime;
            // Integer-hertz equipment tones remain continuous across this wrap.
            if (phase > 4096.) phase -= 4096.;
            const float wave = .58f + .24f * static_cast<float>(std::sin(phase * .67))
                + .16f * static_cast<float>(std::sin(phase * 1.13 + .8));
            const float wind = .55f + .35f * static_cast<float>(std::sin(phase * .23 + 1.7));
            const float outdoorGain = 1.f - insideMix * .78f;
            float left = (seaL * wave * .65f + windL * wind * .11f) * outdoorGain;
            float right = (seaR * wave * .65f + windR * wind * .11f) * outdoorGain;

            // Quiet room/equipment tones replace the muffled exterior. Radio noise
            // is gated into soft irregular swells, never a constant harsh static bed.
            const float room = .003f * insideMix * static_cast<float>(std::sin(phase * 2. * Pi * 43.));
            const float panel = powerMix * panelGain * (.009f * static_cast<float>(std::sin(phase * 2. * Pi * 55.))
                + .003f * static_cast<float>(std::sin(phase * 2. * Pi * 110.)));
            const float swell = Unit(static_cast<float>(std::sin(phase * .41) + std::sin(phase * .73 + 1.4)) - .85f);
            const float radio = .026f * powerMix * radioGain * swell * (windL + windR) * .5f;
            left += room + panel * std::sqrt(.5f * (1.f - panelPan)) + radio * std::sqrt(.5f * (1.f - radioPan));
            right += room + panel * std::sqrt(.5f * (1.f + panelPan)) + radio * std::sqrt(.5f * (1.f + radioPan));
            if (insideMix > .65f && p.master > 0.f && p.ambient > 0.f) {
                structureClock -= SampleTime;
                if (structureClock <= 0.f) {
                    structure = Voice{};
                    structure.active = true;
                    structure.kind = 11;
                    structure.length = CueLength(11);
                    structure.variation = .88f + .24f * Unit(.5f + .5f * Noise(random));
                    const float pan = Pan(Noise(random) * .65f);
                    structure.left = std::sqrt(.5f * (1.f - pan));
                    structure.right = std::sqrt(.5f * (1.f + pan));
                    structureClock = 17.f + 20.f * Unit(.5f + .5f * Noise(random));
                }
            }
            if (structure.active) {
                const float sample = VoiceSample(structure, random) * insideMix;
                left += sample * structure.left;
                right += sample * structure.right;
            }
            left *= p.ambient == 0.f ? 0.f : ambientMix;
            right *= p.ambient == 0.f ? 0.f : ambientMix;

            for (auto& voice : voices) {
                if (!voice.active) continue;
                // Hold the sample clock exactly. A short release of the previous
                // sample avoids a hard cut, without consuming a paused pulse/gap.
                const float value = p.playbackActive ? VoiceSample(voice, random) : voice.lastSample;
                const float sample = value * effectsMix * (playbackMix < .00001f ? 0.f : playbackMix);
                float l = voice.left, r = voice.right;
                if (p.perspective && (voice.kind == 1 || voice.kind == 2)) {
                    const float pan = voice.kind == 1 ? panelPan : radioPan;
                    const float gain = voice.kind == 1 ? panelGain : radioGain;
                    l = gain * std::sqrt(.5f * (1.f - pan));
                    r = gain * std::sqrt(.5f * (1.f + pan));
                }
                left += sample * l;
                right += sample * r;
            }
            // Explicit silence also suppresses cues; avoid a lingering volume ramp.
            const float gain = p.master == 0.f ? 0.f : masterMix;
            samples[frame * 2] = Pcm(left * gain);
            samples[frame * 2 + 1] = Pcm(right * gain);
        }
    }
};
} // namespace

struct Audio::Impl {
    std::atomic<bool> available{ false };
    std::atomic<bool> indoors{ false }, walking{ false };
    std::atomic<float> master{ .65f }, ambient{ .65f }, effects{ .75f };
    std::atomic<unsigned> generation{ 0 };
    HANDLE stopEvent = nullptr;
    std::thread worker;
    std::mutex queueMutex;
    std::array<Command, 32> commands{};
    unsigned commandCount = 0;

    Mixer mixer;
    std::atomic<bool> perspective{ false }, powered{ false }, playbackActive{ true };
    std::atomic<float> radioDistance{ 1000.f }, radioPan{ 0.f }, panelDistance{ 1000.f }, panelPan{ 0.f };

    Impl() {
        stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stopEvent) return;
        try { worker = std::thread([this] { Run(); }); }
        catch (...) { CloseHandle(stopEvent); stopEvent = nullptr; }
    }

    ~Impl() {
        if (stopEvent) SetEvent(stopEvent);
        if (worker.joinable()) worker.join();
        if (stopEvent) CloseHandle(stopEvent);
    }

    void Render(std::array<std::int16_t, Frames * 2>& samples) {
        std::array<Command, 32> pending{};
        unsigned count = 0;
        {
            std::lock_guard<std::mutex> guard(queueMutex);
            count = commandCount;
            for (unsigned i = 0; i < count; ++i) pending[i] = commands[i];
            commandCount = 0;
        }
        MixParameters p;
        p.master = master.load(std::memory_order_relaxed);
        p.ambient = ambient.load(std::memory_order_relaxed);
        p.effects = effects.load(std::memory_order_relaxed);
        p.indoors = indoors.load(std::memory_order_relaxed);
        p.walking = walking.load(std::memory_order_relaxed);
        p.generation = generation.load(std::memory_order_acquire);
        p.perspective = perspective.load(std::memory_order_acquire);
        p.powered = powered.load(std::memory_order_relaxed);
        p.playbackActive = playbackActive.load(std::memory_order_relaxed);
        p.radioDistance = radioDistance.load(std::memory_order_relaxed);
        p.radioPan = radioPan.load(std::memory_order_relaxed);
        p.panelDistance = panelDistance.load(std::memory_order_relaxed);
        p.panelPan = panelPan.load(std::memory_order_relaxed);
        mixer.Render(samples, p, pending.data(), count);
    }

    void Run() noexcept {
        HANDLE readyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
        if (!readyEvent) return;
        WAVEFORMATEX format{};
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = 2;
        format.nSamplesPerSec = SampleRate;
        format.wBitsPerSample = 16;
        format.nBlockAlign = 4;
        format.nAvgBytesPerSec = SampleRate * format.nBlockAlign;
        HWAVEOUT device = nullptr;
        if (waveOutOpen(&device, WAVE_MAPPER, &format,
            reinterpret_cast<DWORD_PTR>(readyEvent), 0, CALLBACK_EVENT) != MMSYSERR_NOERROR) {
            CloseHandle(readyEvent);
            return;
        }
        struct Buffer {
            std::array<std::int16_t, Frames * 2> samples{};
            WAVEHDR header{};
            bool prepared = false;
        };
        std::array<Buffer, BufferCount> buffers{};
        bool healthy = true;
        for (auto& buffer : buffers) {
            buffer.header.lpData = reinterpret_cast<LPSTR>(buffer.samples.data());
            buffer.header.dwBufferLength = static_cast<DWORD>(buffer.samples.size() * sizeof(std::int16_t));
            if (waveOutPrepareHeader(device, &buffer.header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
                healthy = false;
                break;
            }
            buffer.prepared = true;
            Render(buffer.samples);
            if (waveOutWrite(device, &buffer.header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
                healthy = false;
                break;
            }
        }
        available.store(healthy, std::memory_order_release);
        HANDLE events[] = { stopEvent, readyEvent };
        while (healthy) {
            const DWORD result = WaitForMultipleObjects(2, events, FALSE, 100);
            if (result == WAIT_OBJECT_0 || result == WAIT_FAILED) break;
            for (auto& buffer : buffers) {
                if (!(buffer.header.dwFlags & WHDR_DONE)) continue;
                Render(buffer.samples);
                if (waveOutWrite(device, &buffer.header, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
                    healthy = false;
                    break;
                }
            }
        }
        available.store(false, std::memory_order_release);
        waveOutReset(device);
        for (auto& buffer : buffers) {
            if (buffer.prepared) waveOutUnprepareHeader(device, &buffer.header, sizeof(WAVEHDR));
        }
        waveOutClose(device);
        CloseHandle(readyEvent);
    }
};

Audio::Audio() : impl(std::make_unique<Impl>()) {}
Audio::~Audio() = default;

void Audio::Update(float dt, bool indoors, bool walking, float master, float ambient, float effects) {
    // Sample time, not frame duration, clocks the sounds. No allocations or device calls here.
    (void)dt;
    impl->indoors.store(indoors, std::memory_order_relaxed);
    impl->walking.store(walking, std::memory_order_relaxed);
    const float nextMaster = Unit(master), nextEffects = Unit(effects);
    const float previousMaster = impl->master.exchange(nextMaster, std::memory_order_relaxed);
    impl->ambient.store(Unit(ambient), std::memory_order_relaxed);
    const float previousEffects = impl->effects.exchange(nextEffects, std::memory_order_relaxed);
    if ((nextMaster == 0.f && previousMaster > 0.f) || (nextEffects == 0.f && previousEffects > 0.f))
        impl->generation.fetch_add(1, std::memory_order_release);
}

void Audio::SetPerspective(float radioDistance, float radioPan, float panelDistance, float panelPan, bool powered) {
    impl->radioDistance.store(Distance(radioDistance), std::memory_order_relaxed);
    impl->radioPan.store(Pan(radioPan), std::memory_order_relaxed);
    impl->panelDistance.store(Distance(panelDistance), std::memory_order_relaxed);
    impl->panelPan.store(Pan(panelPan), std::memory_order_relaxed);
    impl->powered.store(powered, std::memory_order_relaxed);
    impl->perspective.store(true, std::memory_order_release);
}

void Audio::SetPlaybackActive(bool active) {
    impl->playbackActive.store(active, std::memory_order_relaxed);
}

void Audio::Cue(int kind, float pan) {
    if (kind < 0 || kind > 5 || impl->master.load(std::memory_order_relaxed) == 0.f
        || impl->effects.load(std::memory_order_relaxed) == 0.f) return;
    std::lock_guard<std::mutex> guard(impl->queueMutex);
    if (impl->commandCount < impl->commands.size())
        impl->commands[impl->commandCount++] = Command{ kind, Pan(pan), impl->generation.load(std::memory_order_acquire) };
}

bool Audio::Available() const { return impl->available.load(std::memory_order_acquire); }

} // namespace ultimo
