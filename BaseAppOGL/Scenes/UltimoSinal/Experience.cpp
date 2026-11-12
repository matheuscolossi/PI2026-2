#include "UltimoSinal.h"
#include <algorithm>
#include <cmath>

using ultimo::Screen;
namespace {
float Ease(float t){t=(std::clamp)(t,0.f,1.f);return t*t*(3.f-2.f*t);}
CCamera LookAt(glm::vec3 position,glm::vec3 target,float zoom) {
 const glm::vec3 direction=glm::normalize(target-position);
 CCamera result(position,{0,1,0},glm::degrees(std::atan2(direction.z,direction.x)),glm::degrees(std::asin(direction.y)));
 result.Zoom=zoom;return result;
}
}

bool UltimoSinal::IsMenuBackdrop()const {
 const auto screen=investigation.screen;
 return screen==Screen::Title||((screen==Screen::Options||screen==Screen::Controls||screen==Screen::Help||screen==Screen::ConfirmNew)&&menuReturn==int(Screen::Title));
}
void UltimoSinal::RestorePlayerCamera() {
 if(!experience.menuCameraHeld)return;
 const auto& p=experience.heldPlayerPose;
 camera=CCamera({p.x,p.y,p.z},{0,1,0},p.yaw,p.pitch);camera.Zoom=p.zoom;
 experience.menuCameraHeld=false;skipMouse=true;
}
void UltimoSinal::ResetExperience(bool resumed) {
 // Start/Load have already selected the player's pose. A stale menu camera
 // backup must never overwrite that newly selected position.
 experience.Reset(investigation,resumed);
 atmosphere.Reset(investigation,resumed,camera.Position.x,camera.Position.y,camera.Position.z);
 drama.Reset(investigation);beatMoment=ultimo::AtmosphereMoment::None;velocity=glm::vec3(0);smoothEyeY=camera.Position.y;renderLift=renderZoom=0;
 if(!resumed&&!benchmarkMode)experience.BeginIntro();
}
void UltimoSinal::FinishIntro() {
 if(!experience.introPending)return;
 experience.menuCameraHeld=false;
 camera=CCamera({0,1.7f,17},{0,1,0},-90,0);camera.Zoom=58;
 experience.CompleteIntro();investigation.screen=Screen::Explore;
 target=-1;walking=false;skipMouse=true;SyncCursor();Save();
}
void UltimoSinal::UpdateExperience(float dt) {
 if(benchmarkMode){experience.cabinetAmount=investigation.powerSolved?1.f:0.f;experience.powerPulse=0;return;}
 auto* window=glfwGetCurrentContext();
 if(!window||!glfwGetWindowAttrib(window,GLFW_FOCUSED))return;
 if(IsMenuBackdrop()) {
  if(!experience.menuCameraHeld) {
   experience.heldPlayerPose={camera.Position.x,camera.Position.y,camera.Position.z,camera.Yaw,camera.Pitch,camera.Zoom};
   experience.menuCameraHeld=true;
  }
  experience.AdvanceMenu(dt,true);
  const float t=settings.reduceMotion?0.f:experience.menuTime;
  // A slow, bounded dolly from the access path keeps the lighthouse visible.
  // This presentation camera is restored before control or saving resumes.
  const glm::vec3 position(6.6f+.45f*std::sin(t*.075f),2.8f+.06f*std::sin(t*.11f),13.1f+.35f*std::cos(t*.075f));
  camera=LookAt(position,{-.7f,2.8f,-1.f},54);
  return;
 }
 RestorePlayerCamera();
 if(investigation.screen==Screen::Intro) {
  const bool complete=experience.AdvanceIntro(dt,true);
  const float progress=Ease(experience.introTime/ultimo::IntroDuration);
  if(settings.reduceMotion)camera=CCamera({0,1.7f,17},{0,1,0},-90,0);
  else camera=LookAt(glm::mix(glm::vec3(0,2.4f,19.2f),glm::vec3(0,1.7f,17),progress),glm::mix(glm::vec3(0,2.5f,1),glm::vec3(0,1.7f,1),progress),52.f+6.f*progress);
  if(settings.reduceMotion)camera.Zoom=58;
  if(complete)FinishIntro();
  return;
 }
 const bool explore=investigation.screen==Screen::Explore;
 const bool inLensRoom=explore&&camera.Position.z< -1.05f&&camera.Position.z> -5.6f&&std::abs(camera.Position.x)<4.6f;
 experience.Observe(investigation,inLensRoom);
 experience.Tick(dt,explore,settings.reduceMotion);
 atmosphere.Observe(investigation,camera.Position.x,camera.Position.y,camera.Position.z,explore);
 atmosphere.Tick(dt,explore,experience.activeEvent==ultimo::ExperienceEvent::None,settings.reduceMotion);
}
void UltimoSinal::DrawExperience(bool allowBenchmark) {
 if((benchmarkMode&&!allowBenchmark)||investigation.screen!=Screen::Explore)return;
 const auto display=ImGui::GetIO().DisplaySize;
 if(display.x<1||display.y<1)return;
 const auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_AlwaysAutoResize;
 const auto& chapter=ultimo::DescribeChapter(experience.chapter);
 const float width=(std::min)(460.f,(std::max)(180.f,display.x-40));
 const float titleAlpha=Ease((ultimo::ChapterCardDuration-experience.chapterAge)/1.1f);
 ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.025f,.045f,.055f,.68f));
 ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(.89f,.87f,.80f,1));
 ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(16,12));
 ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,2);
 ImGui::SetNextWindowPos(ImVec2(20,20),ImGuiCond_Always);
 ImGui::SetNextWindowSize(ImVec2(width,0),ImGuiCond_Always);
 ImGui::Begin("##experience_objective",nullptr,flags);
 const int phase=ultimo::spatial::Phase(camera.Position.z,camera.Position.y);
 ImGui::TextColored(ImVec4(.80f,.65f,.39f,1),"ETAPA %d / 3 — %s",phase,ultimo::spatial::PhaseName(phase));

 if(titleAlpha>.01f){ImGui::SetWindowFontScale(1.25f);ImGui::TextWrapped("%s",phase==3&&!investigation.Ready()?"A máquina da luz":chapter.title);ImGui::SetWindowFontScale(1.f);ImGui::Spacing();}
 ImGui::TextWrapped("%s",investigation.submitted?"Seu relatório está preservado no caderno.":investigation.Ready()?"Confronte as evidências no caderno e prepare seu relatório.":phase==3?"Examine o comando óptico e confronte o ensaio com seus registros.":chapter.objective);
 ImGui::End();
 if(experience.activeEvent!=ultimo::ExperienceEvent::None) {
  const auto event=ultimo::DescribeEvent(experience.activeEvent);
  const float fade=Ease(experience.eventAge/.35f)*Ease((ultimo::ExperienceEventDuration-experience.eventAge)/.65f);
  ImGui::PushStyleVar(ImGuiStyleVar_Alpha,fade);
  ImGui::SetNextWindowPos(ImVec2(display.x*.5f,display.y-105),ImGuiCond_Always,ImVec2(.5f,1));
  ImGui::SetNextWindowSize(ImVec2((std::min)(590.f,(std::max)(180.f,display.x-40)),0));
  ImGui::Begin("##experience_event",nullptr,flags);
  ImGui::TextColored(ImVec4(.80f,.65f,.39f,1),"%s",event.label);ImGui::TextWrapped("%s",event.text);
  ImGui::End();ImGui::PopStyleVar();
 }
 if(atmosphere.captionVisible&&experience.activeEvent==ultimo::ExperienceEvent::None) {
  const auto moment=ultimo::DescribeMoment(atmosphere.moment);
  const float fade=Ease(atmosphere.age/.85f)*Ease((ultimo::Atmosphere::MomentDuration-atmosphere.age)/1.15f);
  ImGui::PushStyleVar(ImGuiStyleVar_Alpha,fade);
  ImGui::SetNextWindowPos(ImVec2(display.x*.5f,display.y-155),ImGuiCond_Always,ImVec2(.5f,1));
  ImGui::SetNextWindowSize(ImVec2((std::min)(620.f,(std::max)(180.f,display.x-40)),0));
  ImGui::Begin("##quiet_moment",nullptr,flags);
  ImGui::TextColored(ImVec4(.80f,.65f,.39f,1),"%s",moment.title);
  ImGui::Spacing();ImGui::TextWrapped("%s",moment.text);
  ImGui::End();ImGui::PopStyleVar();
 }
 ImGui::PopStyleVar(2);ImGui::PopStyleColor(2);
}
