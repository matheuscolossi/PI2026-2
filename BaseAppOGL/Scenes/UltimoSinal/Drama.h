#pragma once
#include "Investigation.h"
#include "Audio.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace ultimo {
// Presentation only: nothing here is saved, unlocks a mechanism, adds a clue or
// changes an answer. The storm recedes as the investigation advances, the lamps
// stutter once when power returns, the local lens test floods its own room and
// the overcast thins after the report. Reduced motion removes every flash and
// stutter; the delayed thunder remains audible because it is not a visual cue.
struct Drama {
 static constexpr float StrikeLength=.62f,SurgeLength=2.2f;
 float storm=0;      // 0..1 sheet-lightning fill for sky, fog and ambient
 float lampSurge=0;  // additive lamp energy factor; 0 while the supply is steady
 float lensFlash=0;  // 0..1 amber flood inside the optical room during the test
 float dawn=0;       // 0..1 clearing sky once the report has been submitted
 float untilStrike=7.5f,strikeAge=-1,thunderDelay=-1,thunderPan=0,powerAge=-1;
 float quietFor=60.f; // seconds of running clock since the last strike began
 bool seenPower=false,introStruck=false;
 std::uint32_t seed=0x9e3779b9u;

 void Reset(const Investigation& game){*this=Drama{};seenPower=game.powerSolved;dawn=game.submitted?1.f:0.f;}
 static float SafeDelta(float dt){return std::isfinite(dt)?(std::clamp)(dt,0.f,.1f):0.f;}
 float Random(){seed^=seed<<13;seed^=seed>>17;seed^=seed<<5;return float(seed&0xffffffu)/16777215.f;}
 // Closest before the power returns, distant after the test, gone after the report.
 static float StormLevel(const Investigation& game){
  if(game.submitted)return 0.f;
  if(game.lensSolved)return .30f;
  if(game.DoorOpen())return .55f;
  return game.powerSolved?.75f:1.f;
 }
 // One strike is a fast rise, a decay and a weaker return stroke. Two peaks
 // inside 0.62 s, then at least 16 s of exploration before the next strike.
 static float StrikeShape(float age){
  if(!(age>=0.f)||age>=StrikeLength)return 0.f;
  const float first=std::exp(-age*9.f)*(std::min)(1.f,age/.035f);
  const float later=age-.17f;
  const float second=later>0.f?.55f*std::exp(-later*11.f)*(std::min)(1.f,later/.03f):0.f;
  return (std::min)(1.f,first+second);
 }
 // Filaments catching: three short dips, then a small overshoot that settles.
 static float SurgeShape(float age){
  if(!(age>=0.f)||age>=SurgeLength)return 0.f;
  if(age<.10f)return -.70f;
  if(age<.19f)return .22f;
  if(age<.31f)return -.55f;
  if(age<.46f)return .16f;
  if(age<.58f)return -.30f;
  return .28f*std::exp(-(age-.58f)*3.5f);
 }
 // Same windows as the lamp on the console and the three audible pulses.
 static bool LensPulse(float playback){
  if(!(playback>=LensLeadIn))return false;
  const float t=playback-LensLeadIn;
  return t<2.f*LensPulseSpacing+LensPulseDuration&&std::fmod(t,LensPulseSpacing)<LensPulseDuration;
 }
 static constexpr float MinimumGap=4.f;
 void Strike(float pan){strikeAge=0;quietFor=0;thunderDelay=.9f+1.6f*Random();thunderPan=(std::clamp)(pan,-1.f,1.f);}
 // A story beat (a first look at the photograph, the first arrival in the
 // optical room) may ask for a strike. It is refused while another strike is
 // recent, so two can never stack into a rapid series of flashes.
 bool Beat(float pan){
  if(strikeAge>=0.f||quietFor<MinimumGap)return false;
  Strike(pan);untilStrike=(std::max)(untilStrike,16.f);return true;
 }
 // active: the world clock is running (exploration or prologue).
 // schedule: automatic strikes may be drawn (exploration only).
 // Returns true on the frame the delayed thunder of a strike should be cued.
 bool Tick(float dt,bool active,bool schedule,bool reduceMotion,const Investigation& game,float lensPlayback){
  const float step=SafeDelta(dt);bool thunder=false;
  const float level=StormLevel(game);
  if(active){
   if(schedule&&level>0.f){
    untilStrike-=step*level;
    if(untilStrike<=0.f){
     if(quietFor>=MinimumGap){untilStrike=16.f+18.f*Random();Strike(Random()*1.6f-.8f);}
     else untilStrike=MinimumGap-quietFor;
    }
   }
   quietFor=(std::min)(600.f,quietFor+step);
   if(strikeAge>=0.f){strikeAge+=step;if(strikeAge>=StrikeLength)strikeAge=-1;}
   if(thunderDelay>=0.f){thunderDelay-=step;if(thunderDelay<0.f){thunderDelay=-1;thunder=true;}}
   if(game.powerSolved&&!seenPower){seenPower=true;powerAge=0;}
   if(powerAge>=0.f){powerAge+=step;if(powerAge>=SurgeLength)powerAge=-1;}
  }
  storm=reduceMotion?0.f:StrikeShape(strikeAge)*(.45f+.55f*level);
  lampSurge=reduceMotion?0.f:SurgeShape(powerAge);
  // The lens test and the clearing sky follow their own clocks, so they are
  // not lost while the console or the closing report covers the scene.
  const float lensTarget=lensPlayback<0.f?0.f:reduceMotion?.35f:LensPulse(lensPlayback)?1.f:0.f;
  lensFlash+=(lensTarget-lensFlash)*(1.f-std::exp(-step*(lensTarget>lensFlash?16.f:6.f)));
  if(lensPlayback<0.f&&lensFlash<.002f)lensFlash=0;
  const float dawnTarget=game.submitted?1.f:0.f;
  dawn=reduceMotion?dawnTarget:dawn+(std::clamp)(dawnTarget-dawn,-step*.5f,step*.045f);
  return thunder;
 }
};
}
