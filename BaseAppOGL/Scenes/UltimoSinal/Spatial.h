#pragma once
#include <algorithm>
#include <cmath>
#include <optional>
namespace ultimo::spatial {
inline constexpr float Ground=.095f,Upper=7.295f,Eye=1.605f,Pi=3.14159265359f;
// Two turns, 36 real 20 cm risers. Select the support nearest the previous
// foot height, never an arbitrary turn sharing the same horizontal position.
inline std::optional<float> Support(float x,float z,float previous){
 const float r=std::hypot(x,z+3.f);float angle=std::atan2(z+3.f,x)*180.f/Pi;
 if(angle<0)angle+=360.f;
 const int sector=(std::clamp)(int(angle/20.f),0,17);
 if(r>=.63f&&r<=1.36f){
  for(int turn=0;turn<2;++turn){
   const float h=Ground+.20f*(sector+1+turn*18);
   if(std::abs(h-previous)<=.235f)return h;
  }
 }
 const bool landing=x>.94f&&x<2.45f&&std::abs(z+3.f)<.45f;
 if(previous>Upper-.235f&&r<2.40f&&(r>=1.47f||landing))return Upper;
 if(previous<Ground+.235f){
  if(r<.63f)return std::nullopt;
  if(r<=1.36f&&Ground+.20f*(sector+1)<1.87f)return std::nullopt;
  return Ground;
 }
 return std::nullopt;
}
inline bool ValidPose(float x,float y,float z){
 if(!std::isfinite(y)||y<1.5f||y>9.05f)return false;
 auto h=Support(x,z,y-Eye);return h&&std::abs((*h+Eye)-y)<.13f;
}
inline int Phase(float z,float y){return y>8.65f?3:z<-.70f?2:1;}
inline const char* PhaseName(int phase){return phase==3?"SALA ÓPTICA":phase==2?"ARQUIVO DO FAROL":"SALA DE SERVIÇO";}
}
