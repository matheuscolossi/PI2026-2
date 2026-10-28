#include "Spatial.h"
#include <iostream>
#include <cstdlib>
int main(){using namespace ultimo::spatial;int checks=0;
 auto check=[&](bool ok){++checks;if(!ok){std::cerr<<"FAIL spatial "<<checks<<"\n";std::exit(1);}};
 float foot=Ground;
 for(float x=1.7f;x>1.043f;x-=.01f){auto h=Support(x,-2.81593f,foot);check(bool(h));foot=*h;}
 check(std::abs(foot-(Ground+.2f))<.001f);foot=Ground;
 for(int i=0;i<36;++i){const float a=(10.f+i*20.f)*Pi/180.f;auto h=Support(1.06f*std::cos(a),-3+1.06f*std::sin(a),foot);check(h&&std::abs(*h-(Ground+(i+1)*.2f))<.001f);foot=*h;check(ValidPose(1.06f*std::cos(a),foot+Eye,-3+1.06f*std::sin(a)));}
 check(std::abs(foot-Upper)<.001f);check(Support(1.65f,-3.15f,foot)==Upper);check(Support(1.75f,-4.f,foot)==Upper);check(Support(1.f,-4.5f,foot)==Upper);
 for(int i=35;i>=0;--i){const float a=(10.f+i*20.f)*Pi/180.f;auto h=Support(1.06f*std::cos(a),-3+1.06f*std::sin(a),foot);check(h&&std::abs(*h-(Ground+(i+1)*.2f))<.001f);foot=*h;}
 check(Support(1.7f,-2.8f,foot)==Ground);check(!Support(2.f,-3.f,3.f));check(!Support(0,-3,Ground));check(!ValidPose(5,8.9f,4));check(ValidPose(0,1.7f,17));
 check(Phase(3,1.7f)==1&&Phase(-2,1.7f)==2&&Phase(-3,8.9f)==3);
 std::cout<<checks<<" spatial support checks passed; 36 steps up/down and clear upper landing.\n";
}
