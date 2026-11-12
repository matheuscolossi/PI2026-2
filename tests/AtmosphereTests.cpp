#include "Atmosphere.h"
#include <iostream>
#include <limits>
#include <cstdlib>
int main(){
 using namespace ultimo;int checks=0;
 auto check=[&](bool ok,const char* text){++checks;if(!ok){std::cerr<<"FAIL "<<text<<"\n";std::exit(1);}};
 Investigation game;game.Start();Atmosphere a;
 check(AreaAt(0,1.7f,17)==AcousticArea::Coast,"outside does not trigger a room");
 check(AreaAt(0,1.7f,3)==AcousticArea::Service,"service");
 check(AreaAt(2,1.7f,-3)==AcousticArea::Archive,"archive");
 check(AreaAt(1,5.f,-3)==AcousticArea::Stair,"spiral has metal acoustics");
 check(AreaAt(1,8.9f,-4)==AcousticArea::Optics,"upper room");
 check(AreaAt(8,8.9f,-4)==AcousticArea::Coast,"tower acoustics bounded by glass");
 check(AreaAt(0,std::numeric_limits<float>::quiet_NaN(),0)==AcousticArea::Coast,"invalid pose");
 a.Reset(game,false,0,1.7f,17);a.Observe(game,0,1.7f,17,true);
 check(a.Pending()==0,"no arrival caption before entering");
 a.Observe(game,0,1.7f,3,true);a.Tick(.1f,true,false,false);
 check(a.Pending()==1&&!a.captionVisible,"mechanical event keeps caption priority");
 a.Tick(.1f,true,true,false);check(a.captionVisible&&a.moment==AtmosphereMoment::Service,"service caption");
 const float paused=a.age;a.Tick(99,false,true,false);check(a.age==paused&&!a.captionVisible,"pause freezes caption clock");
 a.Observe(game,2,1.7f,-3,true);a.Tick(.1f,true,true,false);
 check(a.moment==AtmosphereMoment::Archive,"stale area caption replaced on moving rooms");
 for(int i=0;i<100;++i){a.Observe(game,2,1.7f,-3,true);a.Tick(.1f,true,true,false);}
 check(a.moment==AtmosphereMoment::None&&a.Pending()==0,"room entries are not repeatedly queued");
 game.discovered[0]=true;a.Observe(game,2,1.7f,-3,false);a.Tick(.1f,false,true,false);
 check(a.Pending()==1&&a.moment==AtmosphereMoment::None,"discovery waits behind reading screen");
 a.Tick(.1f,true,true,false);check(a.moment==AtmosphereMoment::Diary,"diary reflection");
 for(int i=0;i<35;++i)a.Tick(.1f,true,true,false);
 check(a.quiet>.99f,"quiet moment settles without hard cut");
 const float age=a.age,quiet=a.quiet;a.Tick(std::numeric_limits<float>::quiet_NaN(),true,true,false);
 check(a.age==age&&a.quiet==quiet,"invalid frame delta does not corrupt presentation");
 for(int i=0;i<100;++i){a.Observe(game,2,1.7f,-3,true);a.Tick(.1f,true,true,false);}
 check(a.moment==AtmosphereMoment::None&&a.quiet<.01f,"ambience returns after reflection");
 game.discovered[9]=true;a.Observe(game,2,1.7f,-3,false);a.Tick(.1f,true,true,false);
 check(a.moment==AtmosphereMoment::Letter,"optional letter has a moment but no puzzle prerequisite");
 game.lensSolved=true;a.Observe(game,1,8.9f,-4,true);
 a.Tick(.1f,true,true,false);check(a.resolution>0&&a.resolution<1,"resolution lighting interpolates");
 a.Tick(.1f,true,true,true);check(a.resolution==1,"reduced motion uses stable resolved light");
 a.Reset(game,true,1,8.9f,-4);a.Observe(game,1,8.9f,-4,true);a.Tick(.1f,true,true,false);
 check(a.Pending()==0&&a.moment==AtmosphereMoment::None,"Continue never replays known discoveries or upper arrival");
 game.discovered.fill(false);game.lensSolved=false;game.powerSolved=true;game.discovered[3]=true;
 a.Reset(game,true,2,1.7f,-3);a.Observe(game,1,8.9f,-4,true);a.Tick(.1f,true,true,false);
 check(a.moment==AtmosphereMoment::Optics,"unvisited upper room still introduces itself after Continue");
 check(game.lensSolved==false&&game.discovered[9]==false,"presentation changes no clue or solution");
 std::cout<<checks<<" atmosphere checks passed; phase transitions, captions, pause, resume and reduced motion.\n";
}
