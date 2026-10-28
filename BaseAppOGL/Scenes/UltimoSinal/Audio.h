#pragma once

#include <memory>

namespace ultimo {

// The local lens test and its visual/text alternative share this timing.
inline constexpr float LensPulseDuration = .70f;
inline constexpr float LensPulseSpacing = 1.05f;
inline constexpr float LensLeadIn = .18f;
inline constexpr float LensPlaybackDuration = 3.18f;

// Local PCM synthesis. Device ownership and mixing stay on a background thread.
// Creating Audio is optional: a missing Windows audio device leaves the game usable.
class Audio final {
public:
    Audio();
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;
    Audio(Audio&&) = delete;
    Audio& operator=(Audio&&) = delete;

    // Volume values are clamped to [0, 1]. Call every frame, including paused frames
    // (walking=false). The audio sample clock controls playback, independently of FPS.
    void Update(float dt, bool indoors, bool walking, float master, float ambient, float effects);
    // Distances are world metres; pans are signed dot products with camera.Right.
    // The optional perspective is quiet until called. Equipment ambience requires power.
    // Switch/radio cues follow those emitters; other cues use the explicit pan below.
    void SetPerspective(float radioDistance, float radioPan, float panelDistance, float panelPan, bool powered);
    // Freeze interaction cue clocks while paused or unfocused. Ambient sound remains.
    void SetPlaybackActive(bool active);
    // 0: inspect; 1: switch; 2: receiver tuning; 3: door; 4: invalid; 5: local lens test.
    // pan=-1 is left; pan=+1 is right. Cue never waits for the audio device.
    void Cue(int kind, float pan = 0.f);
    bool Available() const;

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace ultimo
