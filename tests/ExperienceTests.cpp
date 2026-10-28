#include "Experience.h"
#include <cstdlib>
#include <iostream>
#include <limits>

namespace {
int checks=0;
void Check(bool condition,const char* message) {
 ++checks;
 if(!condition){std::cerr<<"FAIL: "<<message<<'\n';std::exit(1);}
}
void Advance(ultimo::Experience& experience,int frames,bool active=true,bool reduced=false) {
 for(int i=0;i<frames;++i)experience.Tick(.1f,active,reduced);
}
void Read(ultimo::Investigation& game,int entry) {
 game.screen=ultimo::Screen::Explore;game.InspectEntry(entry);game.screen=ultimo::Screen::Explore;
}
}
int main() {
 using namespace ultimo;
 Investigation game;game.Start();
 Experience state;state.Reset(game,false);
 Check(ChapterFor(game)==Chapter::Arrival,"new session starts in arrival");
 Check(state.cabinetAmount==0&&state.powerAge<0,"unpowered cabinet closed without visual pulse");
 state.BeginIntro();
 Check(state.introPending&&state.introTime==0,"intro begins before exploration");
 Check(!state.AdvanceIntro(.1f,false)&&state.introTime==0,"pause/focus loss freezes intro");
 Check(!state.AdvanceIntro(-2,true)&&state.introTime==0,"negative time cannot rewind intro");
 Check(!state.AdvanceIntro(std::numeric_limits<float>::quiet_NaN(),true)&&state.introTime==0,"nonfinite intro time ignored");
 state.AdvanceIntro(500,true);Check(state.introTime==.1f,"stall is bounded instead of skipping entire prologue");
 bool ended=false;for(int i=0;i<150&&!ended;++i)ended=state.AdvanceIntro(.1f,true);
 Check(ended&&state.introTime==IntroDuration,"intro ends at declared duration");
 state.CompleteIntro();
 Check(!state.introPending&&state.chapterAge==0,"finish or skip announces arrival and releases intro");
 Check(!state.AdvanceIntro(.1f,true),"completed intro cannot run again");
 state.menuTime=2;state.AdvanceMenu(.1f,false);Check(state.menuTime==2,"inactive menu camera clock frozen");
 state.AdvanceMenu(.1f,true);Check(state.menuTime>2,"active menu camera advances");
 state.Observe(game,false);Check(state.PendingEvents()==0,"merely starting does not fabricate a discovery");

 // Follow the real puzzle guards; presentation must not solve or mutate them.
 Read(game,2);Read(game,5);game.screen=Screen::Panel;game.breakers={false,true,true};
 Check(game.RestorePower(),"legal maintenance puzzle resolves");
 const auto afterPower=game;
 state.Observe(game,false);
 Check(ChapterFor(game)==Chapter::Contact&&state.chapter==Chapter::Contact,"power derives the next chapter");
 Check(state.PendingEvents()==1&&state.powerAge==0,"new power event waits for exploration");
 Advance(state,20,false);
 Check(state.cabinetAmount==0&&state.powerAge==0&&state.activeEvent==ExperienceEvent::None,"device menus freeze motion and event subtitles");
 state.Tick(.1f,true,false);
 Check(state.activeEvent==ExperienceEvent::PowerRestored,"queued power event appears on return to play");
 Check(state.cabinetAmount>0&&state.cabinetAmount<1,"cabinet visibly travels instead of snapping");
 Check(state.powerPulse>0&&state.powerPulse<=.12f,"new power has a bounded nonflashing settle");
 const float cabinet=state.cabinetAmount,age=state.eventAge;
 Advance(state,40,false);
 Check(state.cabinetAmount==cabinet&&state.eventAge==age,"pause freezes event and object animation");
 Advance(state,20);
 Check(state.cabinetAmount==1,"cabinet reaches full open state without overshoot");
 Check(state.powerPulse<.001f,"power settle decays instead of blinking indefinitely");
 Check(game.discovered==afterPower.discovered&&game.breakers==afterPower.breakers&&game.radioSolved==afterPower.radioSolved,"experience never changes puzzle state");
 for(int i=0;i<100;++i)state.Observe(game,false);
 Check(state.PendingEvents()==0,"repeated observation does not repeat power event");

 Read(game,6);game.screen=Screen::Radio;game.radioChannel=6;
 Check(game.TuneRadio(),"legal radio puzzle resolves");state.Observe(game,false);
 Read(game,3);state.Observe(game,false);
 Check(ChapterFor(game)==Chapter::LensRoom,"key unlock derives lens chapter even before all records");
 state.Observe(game,true);state.Observe(game,true);
 Check(state.PendingEvents()==3,"radio key and room events queue once in chronological order");
 Advance(state,60);
 Check(state.activeEvent==ExperienceEvent::RadioReceived,"power event yields to radio event");
 Advance(state,51);
 Check(state.activeEvent==ExperienceEvent::KeyTaken,"key event follows radio event");
 Advance(state,51);
 Check(state.activeEvent==ExperienceEvent::LensRoomEntered,"room event follows key event");
 Advance(state,51);
 Check(state.activeEvent==ExperienceEvent::None&&state.PendingEvents()==0,"event queue drains rather than looping");
 for(int i:{0,1,4,8})Read(game,i);state.Observe(game,true);
 Check(!game.Ready()&&state.chapter==Chapter::LensRoom,"collecting all records alone does not bypass local lens reconstruction");
 game.screen=Screen::LensConsole;game.lensManual=true;game.lensLong={true,false,true};
 const auto draft=game;state.Observe(game,true);Advance(state,10);
 Check(game.lensManual==draft.lensManual&&game.lensLong==draft.lensLong&&!game.lensSolved,"presentation preserves unsolved lens controls without completing the puzzle");
 Check(!game.VerifyLens()&&!game.Ready()&&ChapterFor(game)==Chapter::LensRoom,"an inconsistent reconstruction does not advance the chapter");
 game.lensLong={true,true,true};Check(game.VerifyLens(),"legal local lens reconstruction resolves before report");state.Observe(game,true);
 Check(game.Ready()&&state.chapter==Chapter::Reconstruction,"report chapter requires original prerequisites and verified local reconstruction");
 game.screen=Screen::Conclusion;game.hypothesis=1;game.evidence[7]=game.evidence[8]=true;game.justification="Horários e comando manual confrontados com os registros.";
 Check(game.Submit(),"legal report submits");state.Observe(game,true);
 Check(state.chapter==Chapter::Closed,"submitted report derives epilogue");

 Experience resumed;resumed.Reset(game,true);resumed.Observe(game,true);resumed.Tick(.1f,true,false);
 Check(resumed.chapter==Chapter::Closed&&resumed.resumed&&game.lensSolved,"resume derives chapter from existing save data without losing verified reconstruction");
 Check(!resumed.introPending&&resumed.cabinetAmount==1,"resume keeps powered cabinet open and skips opening cinematic");
 Check(resumed.activeEvent==ExperienceEvent::None&&resumed.PendingEvents()==0&&resumed.powerAge<0,"resume replays no discoveries or power flashes");

 Investigation fresh;fresh.Start();Experience reduced;reduced.Reset(fresh,false);
 fresh.powerSolved=true;reduced.Observe(fresh,false);reduced.Tick(.1f,true,true);
 Check(reduced.cabinetAmount==1&&reduced.powerPulse==0,"reduced motion removes door travel and light settle");
 Check(reduced.activeEvent==ExperienceEvent::PowerRestored,"reduced motion preserves accessible textual feedback");
 Experience skipped;skipped.Reset(fresh,false);skipped.BeginIntro();skipped.AdvanceIntro(.1f,true);skipped.CompleteIntro();
 Check(!skipped.introPending&&skipped.introTime==IntroDuration,"early skip is explicit and deterministic");
 Check(ChapterFor(fresh)==Chapter::Contact&&!fresh.radioSolved&&!fresh.DoorOpen(),"chapter transition cannot grant evidence or bypass radio and key");
 std::cout<<checks<<" experience checks passed\n";
}
