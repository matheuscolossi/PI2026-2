#pragma once
#include "Investigation.h"
#include <algorithm>
#include <array>
#include <cmath>

namespace ultimo {
enum class AcousticArea { Coast, Service, Archive, Optics, Stair };
inline AcousticArea AreaAt(float x,float y,float z) {
 if(!std::isfinite(x)||!std::isfinite(y)||!std::isfinite(z))return AcousticArea::Coast;
 const float radius=std::hypot(x,z+3.f);
 if(y>8.5f&&radius<2.45f)return AcousticArea::Optics;
 if(y>2.05f&&y<8.5f&&radius<1.4f)return AcousticArea::Stair;
 if(std::abs(x)<4.86f&&z> -5.86f&&z<4.86f)return z<-.7f?AcousticArea::Archive:AcousticArea::Service;
 return AcousticArea::Coast;
}
enum class AtmosphereMoment { None, Service, Archive, Optics, Diary, Photograph, Letter, Lens, Aftermath };
struct MomentText {const char* title;const char* text;};
inline MomentText DescribeMoment(AtmosphereMoment moment) {
 switch(moment) {
 case AtmosphereMoment::Service:return {"UMA ROTINA INTERROMPIDA","Uma xícara. Duas cadeiras. A estação conserva os hábitos de quem não voltou."};
 case AtmosphereMoment::Archive:return {"ENTRE OS REGISTROS","O mar fica do lado de fora. Aqui, são os horários que não se encaixam."};
 case AtmosphereMoment::Optics:return {"ACIMA DO SILÊNCIO","Daqui, a luz alcançava os barcos. Quem ficou para acioná-la naquela noite?"};
 case AtmosphereMoment::Diary:return {"A LETRA DE AUGUSTO","A última anotação já não fala de rotina. Alguém tinha algo a perder com aquela fotografia."};
 case AtmosphereMoment::Photograph:return {"UMA IMAGEM PRESERVADA","O nome no casco aproxima este cais da história de Helena."};
 case AtmosphereMoment::Letter:return {"O LUGAR VAZIO","A segunda cadeira permaneceu voltada para o mar. Nenhum mecanismo explica essa espera."};
 case AtmosphereMoment::Lens:return {"DEPOIS DO ENSAIO","O mecanismo respondeu. A pergunta sobre aquela noite continua."};
 case AtmosphereMoment::Aftermath:return {"O QUE PERMANECE","Os registros ficam. O lugar vazio à mesa, também."};
 default:return {"",""};
 }
}

// Presentation only: no new clue, solution or persistent save field. Known
// discoveries initialize the seen flags, so Continue never replays their grief.
struct Atmosphere {
 static constexpr float MomentDuration=7.5f;
 AcousticArea area=AcousticArea::Coast;
 AtmosphereMoment moment=AtmosphereMoment::None;
 float age=0,quiet=0,resolution=0;
 bool captionVisible=false;

 void Reset(const Investigation& game,bool resumed,float x,float y,float z) {
  *this=Atmosphere{};area=AreaAt(x,y,z);
  seenDiary=game.discovered[0];seenPhoto=game.discovered[4];seenLetter=game.discovered[9];
  seenLens=game.lensSolved;seenEnd=game.submitted;resolution=game.lensSolved?1.f:0.f;
  if(resumed){
   seenAreas[1]=game.powerSolved||game.DoorOpen()||area==AcousticArea::Service;
   seenAreas[2]=game.DoorOpen();seenAreas[3]=game.lensSolved||area==AcousticArea::Optics;
  }
 }
 void Observe(const Investigation& game,float x,float y,float z,bool exploring) {
  area=AreaAt(x,y,z);const int index=static_cast<int>(area);
  if(exploring&&index>=1&&index<=3&&!seenAreas[index]){
   seenAreas[index]=true;Queue(index==1?AtmosphereMoment::Service:index==2?AtmosphereMoment::Archive:AtmosphereMoment::Optics);
  }
  auto discover=[&](bool known,bool& seen,AtmosphereMoment event){if(known&&!seen){seen=true;Queue(event);}};
  discover(game.discovered[0],seenDiary,AtmosphereMoment::Diary);
  discover(game.discovered[4],seenPhoto,AtmosphereMoment::Photograph);
  discover(game.discovered[9],seenLetter,AtmosphereMoment::Letter);
  discover(game.lensSolved,seenLens,AtmosphereMoment::Lens);
  discover(game.submitted,seenEnd,AtmosphereMoment::Aftermath);
 }
 void Tick(float dt,bool exploring,bool captionSpace,bool reduceMotion) {
  if(!exploring){captionVisible=false;return;}
  const float step=std::isfinite(dt)?(std::clamp)(dt,0.f,.1f):0.f;
  auto staleArea=[&](AtmosphereMoment event){const int n=static_cast<int>(event);return n>=1&&n<=3&&n!=static_cast<int>(area);};
  if(staleArea(moment)){moment=AtmosphereMoment::None;age=0;}
  if(captionSpace){
   if(moment!=AtmosphereMoment::None){age+=step;if(age>=MomentDuration){moment=AtmosphereMoment::None;age=0;}}
   while(moment==AtmosphereMoment::None&&count){const auto next=pending[0];for(std::size_t i=1;i<count;++i)pending[i-1]=pending[i];--count;if(!staleArea(next)){moment=next;age=0;}}
  }
  captionVisible=captionSpace&&moment!=AtmosphereMoment::None;
  const bool intimate=captionVisible&&moment>=AtmosphereMoment::Diary;
  quiet+=(std::clamp)((intimate?1.f:0.f)-quiet,-step*.6f,step*.6f);
  const float target=seenLens?1.f:0.f;
  resolution=reduceMotion?target:resolution+(std::clamp)(target-resolution,-step*.22f,step*.22f);
 }
 std::size_t Pending()const{return count;}
private:
 std::array<bool,4> seenAreas{};
 bool seenDiary=false,seenPhoto=false,seenLetter=false,seenLens=false,seenEnd=false;
 std::array<AtmosphereMoment,8> pending{};std::size_t count=0;
 void Queue(AtmosphereMoment event){if(count<pending.size())pending[count++]=event;}
};
}
