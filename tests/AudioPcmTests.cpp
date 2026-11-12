#include "../BaseAppOGL/Scenes/UltimoSinal/Audio.cpp"
#include <iostream>
#include <limits>
#include <vector>

namespace {
int checks = 0;
void Check(bool result, const char* what) {
    ++checks;
    if (!result) { std::cerr << "FAILED: " << what << '\n'; std::exit(1); }
}
bool Silent(const std::array<std::int16_t, ultimo::Frames * 2>& block) {
    for (auto value : block) if (value != 0) return false;
    return true;
}
struct Energy { double left = 0., right = 0.; int peak = 0; };
Energy CueEnergy(int kind, float distance, float pan) {
    ultimo::Mixer mixer;
    ultimo::MixParameters p;
    p.master = 1.f; p.ambient = 0.f; p.effects = 1.f; p.perspective = true;
    p.radioDistance = p.panelDistance = distance;
    p.radioPan = p.panelPan = pan;
    std::array<std::int16_t, ultimo::Frames * 2> block{};
    for (int i = 0; i < 160; ++i) mixer.Render(block, p);
    const ultimo::Command command{ kind, 0.f, 0 };
    Energy energy;
    for (int i = 0; i < 55; ++i) {
        mixer.Render(block, p, i == 0 ? &command : nullptr, i == 0 ? 1 : 0);
        for (unsigned frame = 0; frame < ultimo::Frames; ++frame) {
            const int l = block[frame * 2], r = block[frame * 2 + 1];
            energy.left += double(l) * l; energy.right += double(r) * r;
            energy.peak = (std::max)(energy.peak, (std::max)(std::abs(l), std::abs(r)));
        }
    }
    return energy;
}
}

int main() {
    Check(ultimo::DistanceGain(1.f) == 1.f && ultimo::DistanceGain(24.f) == 0.f, "near/far audible radius");
    Check(ultimo::DistanceGain(6.f) > ultimo::DistanceGain(12.f), "distance attenuates monotonically");
    Check(ultimo::DistanceGain(std::numeric_limits<float>::quiet_NaN()) == 0.f, "invalid distance is silent");
    Check(ultimo::Pan(std::numeric_limits<float>::infinity()) == 0.f && ultimo::Pan(8.f) == 1.f, "invalid/bounded pan");
    std::uint32_t random = 12345u;
    for (int kind : { 0, 1, 2, 3, 4, 5, 6, 10, 11 }) {
        ultimo::Voice voice;
        voice.active = true; voice.kind = kind; voice.length = ultimo::CueLength(kind);
        int samples = 0; float peak = 0.f; bool finite = true;
        while (voice.active && samples < 100000) {
            const float sample = ultimo::VoiceSample(voice, random);
            finite = finite && std::isfinite(sample);
            peak = (std::max)(peak, std::fabs(sample)); ++samples;
        }
        Check(finite && !voice.active && peak > .01f && peak < .3f, "all cues finite, bounded and terminate");
        Check(std::abs(samples - int(ultimo::SampleRate * voice.length)) < 12, "cue length follows PCM clock");
        std::cout << "cue=" << kind << " samples=" << samples << " peak=" << peak << '\n';
    }

    ultimo::Voice lens;
    lens.active = true; lens.kind = 5; lens.length = ultimo::LensPlaybackDuration;
    std::vector<float> signal;
    while (lens.active) signal.push_back(ultimo::VoiceSample(lens, random));
    std::vector<float> starts, lengths;
    bool active = false; float start = 0.f;
    constexpr int window = 220; // 10 ms energy window, independent of sinusoid crossings.
    for (unsigned i = 0; i + window < signal.size(); i += window) {
        double energy = 0.;
        for (int j = 0; j < window; ++j) energy += signal[i + j] * signal[i + j];
        const bool now = std::sqrt(energy / window) > .012;
        if (now && !active) { start = float(i) / ultimo::SampleRate; starts.push_back(start); }
        if (!now && active) lengths.push_back(float(i) / ultimo::SampleRate - start);
        active = now;
    }
    Check(starts.size() == 3 && lengths.size() == 3, "lens has exactly three pulses and two silent gaps");
    for (unsigned i = 0; i < starts.size(); ++i) {
        Check(std::fabs(starts[i] - (ultimo::LensLeadIn + float(i) * ultimo::LensPulseSpacing)) < .025f, "lens visual/audio onset alignment");
        Check(lengths[i] > .65f && lengths[i] <= .72f, "lens pulses are long (700 ms)");
        std::cout << "lens pulse=" << i + 1 << " onset=" << starts[i] << " audibleDuration=" << lengths[i] << '\n';
    }

    const auto right = CueEnergy(2, 1.f, .8f), left = CueEnergy(1, 1.f, -.8f), distant = CueEnergy(2, 12.f, .8f);
    Check(right.right > right.left * 6., "radio emitter pans toward camera right");
    Check(left.left > left.right * 6., "panel emitter pans toward camera left");
    Check(distant.left + distant.right < .08 * (right.left + right.right), "distant equipment cue is quieter");
    std::cout << "stereo radio R/L=" << right.right / right.left << " panel L/R=" << left.left / left.right
        << " distant/near energy=" << (distant.left + distant.right) / (right.left + right.right) << '\n';

    ultimo::Mixer mixer;
    ultimo::MixParameters p;
    std::array<std::int16_t, ultimo::Frames * 2> block{};
    p.master = 1.f; p.ambient = 1.f; p.effects = 1.f; p.walking = true;
    p.perspective = true; p.powered = true; p.radioDistance = 1.f; p.panelDistance = 1.f;
    int peak = 0; bool transition = true;
    for (int i = 0; i < 1400; ++i) {
        const float previousMix = mixer.insideMix;
        p.indoors = i >= 180;
        mixer.Render(block, p);
        transition = transition && std::isfinite(mixer.insideMix) && mixer.insideMix >= 0.f && mixer.insideMix <= 1.f
            && std::fabs(mixer.insideMix - previousMix) < .051f;
        for (auto sample : block) peak = (std::max)(peak, std::abs(int(sample)));
    }
    Check(transition && mixer.insideMix > .99f, "interior/exterior crossfade is smooth and settles");
    Check(mixer.structureClock > 0.f && mixer.structureClock < 37.f, "structural ambience stays bounded");
    Check(peak > 0 && peak < 15000, "walking and equipment ambience preserve ample PCM headroom");
    p.master = 0.f;
    const ultimo::Command lensCue{ 5, 0.f, 0 };
    mixer.Render(block, p, &lensCue, 1);
    Check(Silent(block), "master zero is immediately silent, including pending cues");
    p.master = 1.f; p.ambient = 0.f; p.effects = 0.f; p.walking = false;
    mixer.Render(block, p, &lensCue, 1);
    Check(Silent(block), "ambient and effects zero leave no lingering sound");
    p.effects = 1.f; p.generation = 3;
    mixer.Render(block, p, &lensCue, 1);
    Check(Silent(block), "stale cue generation cannot reappear after mute");
    const ultimo::Command freshCue{ 5, 0.f, 3 };
    mixer.Render(block, p, &freshCue, 1);
    p.playbackActive = false;
    const double pausedAt = mixer.voices[0].time;
    for (int i = 0; i < 120; ++i) mixer.Render(block, p);
    Check(mixer.voices[0].time == pausedAt && Silent(block), "pause freezes lens timing and fades sound to silence");
    p.playbackActive = true;
    mixer.Render(block, p);
    Check(mixer.voices[0].time > pausedAt && mixer.voices[0].time < pausedAt + .024f, "lens resumes from held time without skipping a pulse");
    // Phase ambience changes gradually and emotional quieting affects only
    // ambient samples, never the timing/level of the local lens test.
    ultimo::Mixer areas; ultimo::MixParameters areaP;
    areaP.master=1; areaP.ambient=1; areaP.effects=0; areaP.indoors=true;areaP.area=2;
    for(int i=0;i<400;++i)areas.Render(block,areaP);
    Check(areas.archiveMix>.99f&&areas.upperMix<.01f,"archive layer settles");
    areaP.area=3;
    for(int i=0;i<400;++i)areas.Render(block,areaP);
    Check(areas.upperMix>.99f&&areas.archiveMix<.01f,"optical ambience crossfades without stale archive tone");
    auto normal=areas,hushed=areas;double normalEnergy=0,hushedEnergy=0;
    for(int i=0;i<400;++i){
        areaP.focus=0;normal.Render(block,areaP);if(i>300)for(auto v:block)normalEnergy+=double(v)*v;
        areaP.focus=1;hushed.Render(block,areaP);if(i>300)for(auto v:block)hushedEnergy+=double(v)*v;
    }
    Check(normalEnergy>0&&hushedEnergy<normalEnergy*.25,"quiet moment lowers ambient energy smoothly");
    ultimo::Mixer plainCue,focusedCue;ultimo::MixParameters cueP;cueP.master=1;cueP.ambient=0;cueP.effects=1;
    std::array<std::int16_t,ultimo::Frames*2> plainBlock{},focusBlock{};
    for(int i=0;i<300;++i){cueP.focus=0;plainCue.Render(plainBlock,cueP);cueP.focus=1;focusedCue.Render(focusBlock,cueP);}
    const ultimo::Command lensEvent{5,0,0};bool equal=true;
    for(int i=0;i<150;++i){
        cueP.focus=0;plainCue.Render(plainBlock,cueP,i==0?&lensEvent:nullptr,i==0?1:0);
        cueP.focus=1;focusedCue.Render(focusBlock,cueP,i==0?&lensEvent:nullptr,i==0?1:0);equal=equal&&plainBlock==focusBlock;
    }
    Check(equal,"emotional quieting preserves every lens cue sample");
    areaP.focus=std::numeric_limits<float>::quiet_NaN();areas.Render(block,areaP);
    Check(std::isfinite(areas.focusMix),"invalid focus does not contaminate PCM");
    areaP.master=0;areas.Render(block,areaP);Check(Silent(block),"mute remains immediate in upper room");
    std::cout << "ambient/steps peak=" << peak << "; " << checks << " offline PCM checks passed; no device opened.\n";
}
