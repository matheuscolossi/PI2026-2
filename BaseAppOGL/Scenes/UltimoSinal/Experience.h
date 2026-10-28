#pragma once
#include "Investigation.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>

namespace ultimo {
inline constexpr float IntroDuration=13.f;
inline constexpr float ChapterCardDuration=5.5f;
inline constexpr float ExperienceEventDuration=5.f;

enum class Chapter { Arrival, Contact, LensRoom, Reconstruction, Closed };
struct ChapterInfo { const char* label; const char* title; const char* objective; int number; };
inline constexpr std::array<ChapterInfo,5> Chapters{{
 {"CAPÍTULO 01", "A ilha em silêncio", "Investigue a sala de serviço e os sinais deixados no acesso ao cais.",1},
 {"CAPÍTULO 02", "Vozes na estática", "Procure registros da noite e descubra o que o rádio ainda pode revelar.",2},
 {"CAPÍTULO 03", "O arquivo do farol", "Confronte os registros e depois suba à sala óptica pela escada em espiral.",3},
 {"CAPÍTULO 04", "Reconstrução", "Relacione os horários, o funcionamento da luz e o significado do sinal no caderno.",4},
 {"EPÍLOGO", "O relatório", "Seu relatório foi transmitido. Os registros permanecem disponíveis para consulta.",5}
}};
inline Chapter ChapterFor(const Investigation& game) {
 if(game.submitted)return Chapter::Closed;
 if(game.Ready())return Chapter::Reconstruction;
 if(game.DoorOpen())return Chapter::LensRoom;
 if(game.powerSolved)return Chapter::Contact;
 return Chapter::Arrival;
}
inline const ChapterInfo& DescribeChapter(Chapter chapter) { return Chapters[static_cast<std::size_t>(chapter)]; }

enum class ExperienceEvent { None, PowerRestored, RadioReceived, KeyTaken, LensRoomEntered };
struct ExperienceEventInfo { const char* label; const char* text; };
inline ExperienceEventInfo DescribeEvent(ExperienceEvent event) {
 switch(event) {
 case ExperienceEvent::PowerRestored:return {"ENERGIA RESTABELECIDA","A alimentação auxiliar voltou. Uma trava respondeu na sala de serviço."};
 case ExperienceEvent::RadioReceived:return {"REGISTRO RECEBIDO","A transmissão de Brumas foi arquivada no caderno."};
 case ExperienceEvent::KeyTaken:return {"CHAVE DE SERVIÇO","A passagem para o arquivo e o acesso à torre foram liberados."};
 case ExperienceEvent::LensRoomEntered:return {"ARQUIVO DO FAROL","Os documentos desta sala e o mecanismo no alto da torre podem esclarecer o último sinal."};
 default:return {"",""};
 }
}

struct ExperiencePose { float x=0,y=1.7f,z=17,yaw=-90,pitch=0,zoom=58; };

// Presentation state is derived from the existing investigation. It deliberately
// adds no save fields, clues, puzzle prerequisites or authored plot assertions.
struct Experience {
 Chapter chapter=Chapter::Arrival;
 float chapterAge=ChapterCardDuration,menuTime=0,introTime=0;
 float cabinetAmount=0,powerAge=-1,powerPulse=0,eventAge=0;
 bool resumed=false,introPending=false,menuCameraHeld=false;
 ExperiencePose heldPlayerPose{};
 ExperienceEvent activeEvent=ExperienceEvent::None;

 void Reset(const Investigation& game,bool resume) {
  *this=Experience{};chapter=ChapterFor(game);resumed=resume;
  chapterAge=resume?0.f:ChapterCardDuration;
  cabinetAmount=game.powerSolved?1.f:0.f;
  seenPower=game.powerSolved;seenRadio=game.radioSolved;seenKey=game.DoorOpen();
  // A restored game must not replay a discovery or an already-unlocked room.
  seenRoom=resume&&game.DoorOpen();
 }
 void BeginIntro(){introPending=true;introTime=0;chapterAge=ChapterCardDuration;}
 bool AdvanceIntro(float dt,bool active) {
  if(!introPending||!active)return false;
  introTime=(std::min)(IntroDuration,introTime+SafeDelta(dt));
  return introTime>=IntroDuration;
 }
 void CompleteIntro(){introPending=false;introTime=IntroDuration;resumed=false;chapterAge=0;}
 void AdvanceMenu(float dt,bool active) {if(active)menuTime+=SafeDelta(dt);}
 float VisualTime(float worldTime)const{return menuCameraHeld?menuTime:worldTime;}

 // Observe may run while a document or device is open. A resulting event waits
 // for exploration, so its subtitle and cabinet motion are not lost behind UI.
 void Observe(const Investigation& game,bool inLensRoom) {
  const auto next=ChapterFor(game);
  if(next!=chapter){chapter=next;chapterAge=0;resumed=false;}
  if(game.powerSolved&&!seenPower){seenPower=true;powerAge=0;Queue(ExperienceEvent::PowerRestored);}
  if(game.radioSolved&&!seenRadio){seenRadio=true;Queue(ExperienceEvent::RadioReceived);}
  if(game.DoorOpen()&&!seenKey){seenKey=true;Queue(ExperienceEvent::KeyTaken);}
  if(inLensRoom&&game.DoorOpen()&&!seenRoom){seenRoom=true;Queue(ExperienceEvent::LensRoomEntered);}
 }
 void Tick(float dt,bool active,bool reduceMotion) {
  if(!active)return;
  powerPulse=0;
  const float step=SafeDelta(dt);
  chapterAge=(std::min)(ChapterCardDuration,chapterAge+step);
  if(powerAge>=0)powerAge+=step;
  const float target=seenPower?1.f:0.f;
  cabinetAmount=reduceMotion?target:(std::min)(target,cabinetAmount+step*.72f);
  if(!reduceMotion&&powerAge>=0)powerPulse=.12f*std::exp(-3.f*powerAge);
  if(activeEvent!=ExperienceEvent::None) {
   eventAge+=step;
   if(eventAge>=ExperienceEventDuration){activeEvent=ExperienceEvent::None;eventAge=0;}
  }
  if(activeEvent==ExperienceEvent::None&&pendingCount>0) {
   activeEvent=pending[0];eventAge=0;
   for(std::size_t i=1;i<pendingCount;++i)pending[i-1]=pending[i];
   --pendingCount;
  }
 }
 std::size_t PendingEvents()const{return pendingCount;}
 static float SafeDelta(float dt){return std::isfinite(dt)?(std::clamp)(dt,0.f,.1f):0.f;}
private:
 bool seenPower=false,seenRadio=false,seenKey=false,seenRoom=false;
 std::array<ExperienceEvent,4> pending{};
 std::size_t pendingCount=0;
 void Queue(ExperienceEvent event){if(pendingCount<pending.size())pending[pendingCount++]=event;}
};
}
