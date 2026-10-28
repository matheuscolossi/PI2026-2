#pragma once
#include <algorithm>
#include <cmath>
namespace ultimo {
struct Settings{
 float sensitivity=.06f,brightness=1.f,master=.65f,ambient=.55f,effects=.65f;
 bool invertY=false,reduceMotion=false;int quality=1;
 int forward='W',back='S',left='A',right='D',interact='E',journal='J';
 void Reset(){*this=Settings{};}
 float Sensitivity()const{return std::isfinite(sensitivity)?(std::clamp)(sensitivity,.015f,.2f):.06f;}
};
}
