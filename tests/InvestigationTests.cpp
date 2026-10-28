#include "../BaseAppOGL/Scenes/UltimoSinal/Storage.h"
#include "../BaseAppOGL/Scenes/UltimoSinal/Geometry.h"
#include "../BaseAppOGL/Engine/MouseMotion.h"
#include <iostream>
#include <queue>
#include <set>
#include <limits>
static int checks=0;void Check(bool ok,const char* s){++checks;if(!ok){std::cerr<<"FAIL "<<s<<std::endl;std::exit(1);}}
using namespace ultimo;
void Explore(Investigation& g){g.screen=Screen::Explore;}
void Read(Investigation& g,int i){Explore(g);g.InspectEntry(i);Explore(g);}
bool CompletePath(Investigation g){
 for(int i:{0,1,2,5,6,9})Read(g,i);
 g.screen=Screen::Panel;g.breakers={false,true,true};if(!g.RestorePower())return false;
 g.screen=Screen::Radio;g.radioChannel=6;if(!g.TuneRadio())return false;Explore(g);
 for(int i:{3,4,8})Read(g,i);
 if(!g.lensSolved){g.screen=Screen::LensConsole;g.lensManual=true;g.lensLong={true,true,true};if(!g.VerifyLens())return false;}
 return g.Ready();
}
std::string LegacyPayload(const Snapshot& s){auto text=Encode(s);return text.substr(0,text.rfind(" LENS1 "));}
void SelectLens(Investigation& g,int mask){g.lensManual=bool(mask&8);for(int i=0;i<3;++i)g.lensLong[i]=bool(mask&(1<<i));}
int main(){
 Investigation g;Check(!g.Inspect(ClueId::Diary),"no collect title");g.Start();Check(!g.Inspect(ClueId::Count),"invalid classic ID");Check(!g.InspectEntry(10),"invalid entry");Check(!g.Inspect(ClueId::Key),"key locked unpowered");Check(!g.Inspect(ClueId::Photograph),"lens locked");Check(!g.BeginConclusion(),"no premature report");
 g.screen=Screen::Panel;g.breakers={false,true,true};Check(!g.RestorePower(),"no puzzle bypass without evidence");Read(g,5);g.screen=Screen::Panel;Check(!g.RestorePower(),"need wet circuit evidence too");Read(g,2);
 for(int mask=0;mask<8;++mask){Investigation p=g;p.screen=Screen::Panel;p.breakers={bool(mask&1),bool(mask&2),bool(mask&4)};Check(p.RestorePower()==(mask==6),"all circuit combinations");}
 g.screen=Screen::Panel;g.breakers={false,true,true};Check(g.RestorePower(),"power correct");Check(g.helpStage==0,"hint resets stage");
 g.screen=Screen::Radio;g.radioChannel=6;Check(!g.TuneRadio(),"channel needs chart");Read(g,6);
 for(int channel=1;channel<=16;++channel){Investigation r=g;r.screen=Screen::Radio;r.radioChannel=channel;Check(r.TuneRadio()==(channel==6),"all radio channels");}
 g.screen=Screen::Radio;g.radioChannel=6;Check(g.TuneRadio(),"receive transcript");Check(g.discovered[7]&&g.readingReturn==Screen::Radio,"accessible transcript and return");g.CloseReading();Check(g.screen==Screen::Radio,"read back radio");
 for(int i:{0,1,3,4,8})Read(g,i);Check(!g.Ready(),"critical observations still require local reconstruction");Check(g.Count()==5,"five classics preserved");Read(g,0);Check(g.Count()==5,"no duplicate");
 // The diagnostic must reconcile real observations, rather than unlock from guessed controls.
 for(int missing:{-1,3,4,8}){
  Investigation p=g;p.screen=Screen::LensConsole;p.lensManual=true;p.lensLong={true,true,true};
  if(missing==-1)p.powerSolved=false;else p.discovered[missing]=false;
  Check(!p.VerifyLens(),"lens requires service access and all three source records");
 }
 {Investigation p=g;p.radioSolved=false;p.screen=Screen::LensConsole;p.lensManual=true;p.lensLong={true,true,true};Check(!p.VerifyLens(),"lens requires actual solved radio");}
 {Investigation p=g;p.screen=Screen::Explore;p.lensManual=true;p.lensLong={true,true,true};Check(!p.VerifyLens(),"lens cannot be solved outside console");}
 for(int mask=0;mask<16;++mask){
  Investigation p=g;p.screen=Screen::LensConsole;SelectLens(p,mask);const auto records=p.discovered;
  Check(p.VerifyLens()==(mask==15),"all mode and pulse combinations");
  Check(p.discovered==records,"diagnostic never invents a historical observation");
  Check(p.lensSolved==(mask==15),"only coherent reconstruction is recorded");
 }
 g.screen=Screen::LensConsole;SelectLens(g,15);g.helpStage=3;Check(g.VerifyLens(),"manual long sequence verified");Check(g.helpStage==0&&g.Ready(),"verified reconstruction enables report and resets hint");
 Check(!g.VerifyLens()&&g.lensSolved,"solved reconstruction is not repeated");
 Explore(g);g.ToggleJournal();g.ReadEntry(8);g.CloseReading();Check(g.screen==Screen::Journal,"reread returns journal");Explore(g);Check(g.BeginConclusion(),"report enabled");g.hypothesis=1;g.evidence[0]=g.evidence[7]=true;g.justification="  \n";Check(!g.Submit(),"blank blocked");g.justification="Queda às 23h10; comando manual às 23h17.";Check(g.Submit()&&g.Correct(),"report ending");
 // Consulting evidence after feedback must not enable rewriting the original report.
 const auto submittedHypothesis=g.hypothesis;const auto submittedEvidence=g.evidence;const auto submittedText=g.justification;
 g.screen=Screen::Journal;Check(g.ReadEntry(7),"submitted transcript remains readable");g.CloseReading();Check(g.screen==Screen::Journal,"submitted reading returns to journal");
 Explore(g);Check(!g.BeginConclusion(),"feedback cannot reopen report");Check(g.screen==Screen::Explore,"rejected reopening does not enter conclusion");
 g.screen=Screen::Conclusion;Check(!g.CanSubmit(),"submitted report cannot be submitted even if screen is forced");Check(!g.Submit(),"second submission rejected");
 Check(g.submitted&&g.hypothesis==submittedHypothesis&&g.evidence==submittedEvidence&&g.justification==submittedText,"consultation preserves original report");g.screen=Screen::Complete;
 Investigation fresh=g;fresh.Start();Check(!fresh.submitted&&fresh.hypothesis==-1&&fresh.justification.empty(),"new investigation explicitly clears frozen report");
 Snapshot snap;snap.game=g;snap.complete=true;snap.elapsed=531.4;Snapshot restored;auto encoded=Encode(snap);Check(Decode(encoded,restored),"save round trip");Check(restored.game.Correct()&&restored.game.submitted&&restored.elapsed==snap.elapsed,"ending preserved");
 Check(restored.game.lensSolved&&restored.game.lensManual&&std::all_of(restored.game.lensLong.begin(),restored.game.lensLong.end(),[](bool v){return v;}),"new solved diagnostic round trip");
 restored.game.screen=Screen::Explore;Check(!restored.game.BeginConclusion(),"loaded submitted report remains frozen");restored.game.screen=Screen::Conclusion;Check(!restored.game.Submit(),"loaded report cannot be resubmitted");
 Check(!Decode(encoded+" junk",restored),"trailing corruption rejected");Check(!Decode("garbage",restored),"invalid payload");snap.x=100;Check(!Decode(Encode(snap),restored),"unsafe position rejected");snap.x=0;snap.game.powerSolved=false;Check(!Decode(Encode(snap),restored),"inconsistent progress rejected");
 // Existing ULTIMO3 payloads remain loadable; only already submitted reports are grandfathered.
 snap.game=g;const auto legacyComplete=LegacyPayload(snap);
 Check(Decode(legacyComplete,restored),"legacy completed report migrates");
 Check(restored.game.submitted&&restored.game.lensSolved&&restored.game.lensManual&&restored.game.lensLong==std::array<bool,3>{true,true,true},"legacy completed report grandfathered");
 Check(restored.game.hypothesis==g.hypothesis&&restored.game.evidence==g.evidence&&restored.game.justification==g.justification,"migration preserves immutable report exactly");
 restored.game.screen=Screen::Conclusion;Check(!restored.game.Submit(),"migrated submitted report cannot be rewritten");
 Snapshot oldPartial;oldPartial.game=g;oldPartial.game.submitted=false;oldPartial.complete=false;oldPartial.game.hypothesis=2;oldPartial.game.justification="Rascunho: \"LENS1\" e evidências ainda em análise.";
 Check(Decode(LegacyPayload(oldPartial),restored),"legacy complete observations but unfinished report migrates");
 Check(!restored.game.lensSolved&&!restored.game.lensManual&&restored.game.lensLong==std::array<bool,3>{false,false,false}&&!restored.game.Ready(),"legacy unfinished investigation requires new diagnostic");
 Check(restored.game.discovered==oldPartial.game.discovered&&restored.game.justification==oldPartial.game.justification&&restored.game.hypothesis==2,"legacy unfinished observations and report draft preserved");
 Snapshot early;early.game.Start();Read(early.game,2);Check(Decode(LegacyPayload(early),restored)&&restored.game.discovered[2]&&!restored.game.lensSolved,"early legacy progress migrates without fabricated records");
 // Every new control draft survives save/load, including incorrect trials.
 for(int mask=0;mask<16;++mask){Snapshot draft=oldPartial;draft.game.lensSolved=false;SelectLens(draft.game,mask);Check(Decode(Encode(draft),restored),"lens draft decodes");Check(restored.game.lensManual==draft.game.lensManual&&restored.game.lensLong==draft.game.lensLong&&!restored.game.lensSolved,"mode and all pulse draft controls preserved");}
 const auto legacyPrefix=LegacyPayload(oldPartial);
 for(const auto* extension:{" LENS2 0 0 0 0 0"," LENS1"," LENS1 0 1 1 1"," LENS1 0 1 1 1 1 0"," LENS1 0 2 1 1 1"," LENS1 0 1 1 -1 1"," LENS1 1 0 1 1 1"," LENS1 1 1 1 0 1"," LENS1 0 0 0 0 0 LENS1 0 0 0 0 0"})Check(!Decode(legacyPrefix+extension,restored),"unknown truncated trailing nonboolean or incoherent extension rejected");
 for(int missing:{3,4,7,8}){Snapshot inconsistent=oldPartial;inconsistent.game.lensSolved=true;SelectLens(inconsistent.game,15);inconsistent.game.discovered[missing]=false;Check(!Decode(Encode(inconsistent),restored),"solved save requires observed sources and access");}
 Snapshot inconsistent=oldPartial;inconsistent.game.lensSolved=true;SelectLens(inconsistent.game,15);inconsistent.game.radioSolved=false;Check(!Decode(Encode(inconsistent),restored),"solved save requires real radio state");
 inconsistent=oldPartial;inconsistent.complete=true;inconsistent.game.lensSolved=false;SelectLens(inconsistent.game,15);Check(!Decode(Encode(inconsistent),restored),"new completed save cannot bypass diagnostic");
 Investigation human=g;human.submitted=false;human.screen=Screen::Conclusion;human.hypothesis=2;human.justification="Uma interpretação alternativa para discussão com o professor.";Check(human.Submit()&&!human.Correct(),"human written conclusion is not autograded or blocked by chosen hypothesis");
 // Exhaust reachable observations, mechanisms and local-console drafts in every order.
 std::queue<Investigation> queue;std::set<unsigned long> seen;Investigation initial;initial.Start();queue.push(initial);int states=0;
 while(!queue.empty()){
  auto state=queue.front();queue.pop();unsigned long signature=0;for(int i=0;i<10;++i)if(state.discovered[i])signature|=1ul<<i;if(state.powerSolved)signature|=1ul<<10;if(state.radioSolved)signature|=1ul<<11;if(state.lensSolved)signature|=1ul<<12;if(state.lensManual)signature|=1ul<<13;for(int i=0;i<3;++i)if(state.lensLong[i])signature|=1ul<<(14+i);
  if(!seen.insert(signature).second)continue;++states;Check(CompletePath(state),"every reachable state retains a completion path");
  Snapshot partial;partial.game=state;Snapshot loaded;Check(Decode(Encode(partial),loaded),"save valid at each progress stage");
  for(int i=0;i<10;++i){auto next=state;Explore(next);if(next.InspectEntry(i)){Explore(next);queue.push(next);}}
  if(!state.powerSolved){auto next=state;next.screen=Screen::Panel;next.breakers={false,true,true};if(next.RestorePower()){Explore(next);queue.push(next);}}
  if(!state.radioSolved){auto next=state;next.screen=Screen::Radio;next.radioChannel=6;if(next.TuneRadio()){Explore(next);queue.push(next);}}
  if(state.powerSolved&&state.DoorOpen()&&!state.lensSolved){
   for(int control=0;control<4;++control){auto next=state;if(control==3)next.lensManual=!next.lensManual;else next.lensLong[control]=!next.lensLong[control];queue.push(next);}
   auto next=state;next.screen=Screen::LensConsole;if(next.VerifyLens()){Explore(next);queue.push(next);}
  }
 }
 MouseMotion mouse;mouse.Add(0,0);mouse.Add(2,3);mouse.Add(7,5);Check(mouse.dx==7&&mouse.dy== -5,"all mouse events");mouse.Clear();Check(mouse.dx==0,"mouse consumed");mouse.Reset();mouse.Add(999,999);Check(mouse.dx==0,"recapture reset");
 Bounds wall{{0,1,0},{.001f,2,3}};Check(SegmentHits({-1,1,0},{1,1,0},wall),"thin wall occlusion");Check(!SegmentHits({-1,3,0},{1,3,0},wall),"clear segment");Check(BlocksWalker(0,0,wall),"walker blocked");Settings settings;settings.sensitivity=std::numeric_limits<float>::quiet_NaN();Check(settings.Sensitivity()==.06f,"safe sensitivity");
 std::cout<<checks<<" verificacoes aprovadas; "<<states<<" estados de progresso alcancaveis; 8 circuitos, 16 canais e 16 configuracoes da lente."<<std::endl;
}
