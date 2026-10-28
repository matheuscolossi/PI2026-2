#pragma once
#include "Investigation.h"
#include "Settings.h"
#include "Spatial.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cstdint>
namespace ultimo {
struct Snapshot{Investigation game;float x=0,y=1.7f,z=17,yaw=-90,pitch=0;double elapsed=0;bool complete=false;};
inline std::string Encode(const Snapshot& s){
 std::ostringstream out;out<<std::setprecision(12);
 for(bool v:s.game.discovered)out<<v<<' ';for(bool v:s.game.breakers)out<<v<<' ';for(bool v:s.game.evidence)out<<v<<' ';
 out<<s.game.powerSolved<<' '<<s.game.radioSolved<<' '<<s.game.radioChannel<<' '<<s.game.hypothesis<<' '<<s.game.helpStage<<' '<<s.complete<<' '<<s.x<<' '<<s.y<<' '<<s.z<<' '<<s.yaw<<' '<<s.pitch<<' '<<s.elapsed<<' '<<std::quoted(s.game.justification);
 out<<" LENS1 "<<s.game.lensSolved<<' '<<s.game.lensManual;for(bool v:s.game.lensLong)out<<' '<<v;return out.str();
}
inline bool Decode(const std::string& text,Snapshot& result){
 Snapshot s;std::istringstream in(text);auto bit=[&](bool& b){int v;if(!(in>>v)||v<0||v>1)return false;b=v!=0;return true;};
 for(auto& v:s.game.discovered)if(!bit(v))return false;for(auto& v:s.game.breakers)if(!bit(v))return false;for(auto& v:s.game.evidence)if(!bit(v))return false;
 if(!bit(s.game.powerSolved)||!bit(s.game.radioSolved))return false;
 if(!(in>>s.game.radioChannel>>s.game.hypothesis>>s.game.helpStage)||!bit(s.complete))return false;
 if(!(in>>s.x>>s.y>>s.z>>s.yaw>>s.pitch>>s.elapsed>>std::quoted(s.game.justification)))return false;
 in>>std::ws;
 if(!in.eof()){
  std::string extension;if(!(in>>extension)||extension!="LENS1"||!bit(s.game.lensSolved)||!bit(s.game.lensManual))return false;
  for(auto& v:s.game.lensLong)if(!bit(v))return false;
  in>>std::ws;if(!in.eof())return false;
 }else if(s.complete){
  // Preserve immutable reports submitted before the local diagnostic was introduced.
  s.game.lensSolved=true;s.game.lensManual=true;s.game.lensLong={true,true,true};
 }
 if(!std::isfinite(s.x)||!std::isfinite(s.y)||!std::isfinite(s.z)||!std::isfinite(s.yaw)||!std::isfinite(s.pitch)||!std::isfinite(s.elapsed)||std::abs(s.x)>12.5f||std::abs(s.z)>20.5f||(s.y<1.5f||s.y>9.05f||(s.y>1.9f&&!spatial::ValidPose(s.x,s.y,s.z)))||std::abs(s.pitch)>89||std::abs(s.yaw)>1e8||s.elapsed<0||s.elapsed>1e8||s.game.hypothesis< -1||s.game.hypothesis>2||s.game.radioChannel<1||s.game.radioChannel>16||s.game.helpStage<0||s.game.helpStage>3||s.game.justification.size()>2047)return false;
 if(s.game.powerSolved&&(!s.game.discovered[2]||!s.game.discovered[5]||s.game.breakers[0]||!s.game.breakers[1]||!s.game.breakers[2]))return false;
 if(s.game.radioSolved&&(!s.game.powerSolved||!s.game.discovered[6]||!s.game.discovered[7]))return false;
 if(s.game.discovered[7]!=s.game.radioSolved)return false;
 if(s.game.DoorOpen()&&!s.game.powerSolved)return false;
 if(s.y>1.9f&&!s.game.DoorOpen())return false;
 if((s.game.discovered[4]||s.game.discovered[8])&&!s.game.DoorOpen())return false;
 if(s.game.lensSolved&&(!s.game.powerSolved||!s.game.DoorOpen()||!s.game.radioSolved||!s.game.discovered[4]||!s.game.discovered[8]||!s.game.lensManual||!std::all_of(s.game.lensLong.begin(),s.game.lensLong.end(),[](bool v){return v;})))return false;
 s.game.screen=Screen::Conclusion;if(s.complete&&!s.game.CanSubmit())return false;
 s.game.submitted=s.complete;s.game.screen=s.complete?Screen::Complete:Screen::Explore;result=s;return true;
}
inline std::uint64_t Fingerprint(const std::string& text){std::uint64_t h=1469598103934665603ull;for(unsigned char c:text){h^=c;h*=1099511628211ull;}return h;}
inline bool ReadSave(const std::filesystem::path& path,Snapshot& snapshot){
 std::ifstream file(path,std::ios::binary);if(!file)return false;
 std::string header;std::getline(file,header);std::uint64_t checksum;std::istringstream hi(header);std::string format;if(!(hi>>format>>checksum)||format!="ULTIMO3")return false;
 std::string payload((std::istreambuf_iterator<char>(file)),{});if(payload.size()>8192||Fingerprint(payload)!=checksum)return false;return Decode(payload,snapshot);
}
}
