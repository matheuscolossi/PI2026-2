#include "Drama.h"
#include <iostream>
#include <limits>
#include <cstdlib>
int main(){
 using namespace ultimo;int checks=0;
 auto check=[&](bool ok,const char* text){++checks;if(!ok){std::cerr<<"FAIL "<<text<<"\n";std::exit(1);}};
 const float nan=std::numeric_limits<float>::quiet_NaN();
 Investigation game;game.Start();

 // The storm follows the investigation; it never drives it.
 check(Drama::StormLevel(game)==1.f,"storm is closest before the power returns");
 Investigation later=game;later.powerSolved=true;
 check(Drama::StormLevel(later)==.75f,"storm eases once the power is back");
 later.discovered[3]=true;check(Drama::StormLevel(later)==.55f,"storm eases again with the archive open");
 later.lensSolved=true;check(Drama::StormLevel(later)==.30f,"storm is distant after the local lens test");
 later.submitted=true;check(Drama::StormLevel(later)==0.f,"no storm after the report");

 // One strike: bounded, finite, at most two peaks, silent outside its window.
 check(Drama::StrikeShape(-1.f)==0.f&&Drama::StrikeShape(Drama::StrikeLength)==0.f&&Drama::StrikeShape(nan)==0.f,"strike is zero outside its window");
 {
  int peaks=0;float previous=0,before=0;bool bounded=true;
  for(int i=0;i<=600;++i){
   const float value=Drama::StrikeShape(Drama::StrikeLength*float(i)/600.f);
   bounded=bounded&&std::isfinite(value)&&value>=0.f&&value<=1.f;
   if(i>1&&previous>before&&previous>value&&previous>.05f)++peaks;
   before=previous;previous=value;
  }
  check(bounded,"strike brightness stays within 0..1");
  check(peaks>=1&&peaks<=2,"a strike has at most two peaks");
 }
 check(Drama::SurgeShape(-1.f)==0.f&&Drama::SurgeShape(Drama::SurgeLength)==0.f&&Drama::SurgeShape(nan)==0.f,"lamp surge is zero outside its window");
 {
  bool bounded=true;
  for(int i=0;i<400;++i){const float value=Drama::SurgeShape(Drama::SurgeLength*float(i)/400.f);bounded=bounded&&value>-.95f&&value<.5f;}
  check(bounded,"lamp surge never inverts or doubles the supply");
 }

 // The lens glow uses the windows of the console lamp and the audible pulses.
 check(!Drama::LensPulse(-1.f)&&!Drama::LensPulse(LensLeadIn-.01f)&&!Drama::LensPulse(nan),"no pulse before the lead-in");
 check(Drama::LensPulse(LensLeadIn+.10f)&&Drama::LensPulse(LensLeadIn+LensPulseSpacing+.10f)&&Drama::LensPulse(LensLeadIn+2.f*LensPulseSpacing+.10f),"three long pulses");
 check(!Drama::LensPulse(LensLeadIn+LensPulseDuration+.05f)&&!Drama::LensPulse(LensLeadIn+LensPulseSpacing+LensPulseDuration+.05f),"gaps between pulses");
 check(!Drama::LensPulse(LensLeadIn+2.f*LensPulseSpacing+LensPulseDuration+.02f)&&!Drama::LensPulse(LensPlaybackDuration),"no fourth pulse");

 // Two minutes of exploration at 60 fps before the power returns.
 {
  Drama d;d.Reset(game);int strikes=0,thunders=0;float sinceStrike=1000.f,shortestGap=1000.f,maxStorm=0;bool pending=false,delayOk=true,bounded=true;float delay=0;
  for(int frame=0;frame<120*60;++frame){
   const bool wasIdle=d.strikeAge<0.f;
   const bool thunder=d.Tick(1.f/60.f,true,true,false,game,-1.f);
   sinceStrike+=1.f/60.f;if(pending)delay+=1.f/60.f;
   if(wasIdle&&d.strikeAge>=0.f){++strikes;if(strikes>1)shortestGap=(std::min)(shortestGap,sinceStrike);sinceStrike=0;pending=true;delay=0;}
   if(thunder){++thunders;delayOk=delayOk&&pending&&delay>.85f&&delay<2.6f;pending=false;}
   maxStorm=(std::max)(maxStorm,d.storm);
   bounded=bounded&&std::isfinite(d.storm)&&d.storm>=0.f&&d.storm<=1.f;
  }
  check(bounded,"storm level stays within 0..1");
  check(strikes>=3&&strikes<=8,"a few strikes in two minutes, never a strobe");
  check(shortestGap>=15.9f,"at least sixteen seconds between strikes");
  check(thunders==strikes||thunders==strikes-1,"each strike is followed by one thunder");
  check(delayOk,"thunder trails its strike by about one to two and a half seconds");
  check(maxStorm>.5f,"a strike is visible");
 }
 // Story beats never stack flashes.
 {
  Drama d;d.Reset(game);
  check(d.Beat(.3f)&&d.strikeAge==0.f,"a beat strikes when the sky has been quiet");
  check(!d.Beat(.3f),"no second strike while one is in progress");
  for(int i=0;i<200;++i)d.Tick(1.f/60.f,true,false,false,game,-1.f);
  check(!d.Beat(.3f),"no strike within four seconds of the last one");
  for(int i=0;i<60;++i)d.Tick(1.f/60.f,true,false,false,game,-1.f);
  check(d.Beat(.3f)&&d.untilStrike>=16.f,"a later beat strikes and postpones the automatic one");
  d.untilStrike=.01f;d.Tick(1.f/60.f,true,true,false,game,-1.f);
  check(d.quietFor<1.f&&d.untilStrike>3.f,"an automatic strike due right after a beat is deferred");
 }
 // Paused clock, prologue and reduced motion.
 {
  Drama d;d.Reset(game);const float before=d.untilStrike;
  for(int i=0;i<600;++i)d.Tick(.1f,false,false,false,game,-1.f);
  check(d.untilStrike==before&&d.storm==0.f&&d.strikeAge<0.f,"a document or pause freezes the storm");
  for(int i=0;i<600;++i)d.Tick(.1f,true,false,false,game,-1.f);
  check(d.untilStrike==before&&d.strikeAge<0.f,"the prologue draws no automatic strike");
  d.Strike(-4.f);check(d.thunderPan==-1.f&&d.strikeAge==0.f,"a scripted strike is bounded");
  d.Tick(.02f,true,false,false,game,-1.f);check(d.storm>0.f,"a scripted strike lights the prologue");
 }
 {
  Drama d;d.Reset(game);int thunders=0;bool dark=true;Investigation live=game;
  for(int frame=0;frame<90*60;++frame){
   if(frame==600)live.powerSolved=true;
   if(d.Tick(1.f/60.f,true,true,true,live,frame>3000&&frame<3190?float(frame-3000)/60.f:-1.f))++thunders;
   dark=dark&&d.storm==0.f&&d.lampSurge==0.f&&d.lensFlash<=.36f;
  }
  check(dark,"reduced motion removes flashes, the lamp stutter and the pulsing flood");
  check(thunders>=2,"reduced motion keeps the thunder audible");
 }
 // Power: one stutter when it returns during play, none when a save is resumed.
 {
  Drama d;d.Reset(game);Investigation live=game;
  d.Tick(.016f,true,true,false,live,-1.f);check(d.lampSurge==0.f&&d.powerAge<0.f,"steady supply before the puzzle");
  live.powerSolved=true;
  d.Tick(.016f,false,false,false,live,-1.f);check(d.powerAge<0.f,"the stutter waits for exploration, behind no panel");
  d.Tick(.016f,true,true,false,live,-1.f);check(d.powerAge>=0.f&&d.lampSurge<0.f,"lamps dip as the power returns");
  for(int i=0;i<200;++i)d.Tick(.016f,true,true,false,live,-1.f);
  check(d.powerAge<0.f&&d.lampSurge==0.f,"supply settles and stays steady");
  Drama resumed;resumed.Reset(live);for(int i=0;i<10;++i)resumed.Tick(.016f,true,true,false,live,-1.f);
  check(resumed.powerAge<0.f&&resumed.lampSurge==0.f,"Continue does not replay the power stutter");
 }
 // Lens glow follows the playback even while the console covers the scene.
 {
  Drama d;d.Reset(game);
  for(int i=0;i<30;++i)d.Tick(1.f/60.f,false,false,false,game,LensLeadIn+.2f);
  check(d.lensFlash>.9f&&d.lensFlash<=1.f,"glow rises inside a pulse");
  for(int i=0;i<12;++i)d.Tick(1.f/60.f,false,false,false,game,LensLeadIn+LensPulseDuration+.1f);
  check(d.lensFlash<.5f,"glow falls in the gap");
  for(int i=0;i<600;++i)d.Tick(1.f/60.f,false,false,false,game,-1.f);
  check(d.lensFlash==0.f,"glow ends with the playback");
 }
 // Clearing sky after the report.
 {
  Drama d;d.Reset(game);Investigation closed=game;
  for(int i=0;i<600;++i)d.Tick(.1f,true,true,false,closed,-1.f);
  check(d.dawn==0.f,"no clearing before the report");
  closed.submitted=true;float previous=0;bool monotonic=true;
  for(int i=0;i<300;++i){d.Tick(.1f,false,false,false,closed,-1.f);monotonic=monotonic&&d.dawn>=previous&&d.dawn<=1.f;previous=d.dawn;}
  check(monotonic&&d.dawn==1.f,"sky clears gradually and completely behind the closing report");
  check(d.storm==0.f,"no lightning after the report");
  Drama quick;quick.Reset(game);quick.Tick(.016f,false,false,true,closed,-1.f);
  check(quick.dawn==1.f,"reduced motion skips the transition");
  Drama resumed;resumed.Reset(closed);check(resumed.dawn==1.f,"a finished save reopens under the cleared sky");
 }
 // Invalid frame times never corrupt the state.
 {
  Drama d;d.Reset(game);
  for(float dt:{nan,-1.f,std::numeric_limits<float>::infinity(),1000.f})d.Tick(dt,true,true,false,game,nan);
  check(std::isfinite(d.storm)&&std::isfinite(d.untilStrike)&&std::isfinite(d.lensFlash)&&std::isfinite(d.dawn)&&d.untilStrike>7.f,"invalid or huge frame times are clamped");
 }
 Investigation untouched;untouched.Start();
 check(game.discovered==untouched.discovered&&game.powerSolved==untouched.powerSolved&&game.radioSolved==untouched.radioSolved&&game.lensSolved==untouched.lensSolved&&game.submitted==untouched.submitted&&game.hypothesis==untouched.hypothesis,"presentation changes no clue, mechanism or answer");
 std::cout<<checks<<" drama checks passed; storm cadence, power stutter, lens glow, clearing sky and reduced motion.\n";
}
