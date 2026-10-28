#include "UltimoSinal.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
using ultimo::Screen;
namespace {
// Conservative bounds of the leaf at 90..100 degrees. Keep the central passage clear.
const ultimo::Bounds OpenLeaf{{-1.1384f,1.35f,.4553f},{.47f,2.7f,1.94f}};
}
UltimoSinal::UltimoSinal(){
 camera.Zoom=58;cube.generateCube(1);cube.setupBuffers();cylinder.generateCylinder(1,1);cylinder.setupBuffers();cone.generateCone(1,1);cone.setupBuffers();waterPlane.generatePlane(1,1);waterPlane.setupBuffers();
 shader.LoadShader("Inspection","Scenes/UltimoSinal/Prototype.vert","Scenes/UltimoSinal/Inspection.frag");
 shader.LoadShader("Prototype","Scenes/UltimoSinal/Prototype.vert","Scenes/UltimoSinal/Prototype.frag");shader.LoadShader("SkyBox","Scenes/Common/SkyBox.vert","Scenes/UltimoSinal/Sky.frag");
 skybox=std::make_unique<CSkybox>(std::vector<std::string>{"Scenes/Skybox/right.bmp","Scenes/Skybox/left.bmp","Scenes/Skybox/top.bmp","Scenes/Skybox/bottom.bmp","Scenes/Skybox/front.bmp","Scenes/Skybox/back.bmp"});
 textures.LoadTextureMipMap(0,"Scenes/Textures/grass.bmp");textures.LoadTextureMipMap(1,"Scenes/Textures/rock.jpg");textures.LoadTextureMipMap(2,"Scenes/Textures/rockNormal.jpg");
 for(int i=0;i<5;++i)interactionPositions[i]=cluePositions[i];
 interactionPositions[5]={-2.5f,1.15f,.25f};interactionPositions[6]={1.7f,1.35f,4.7f};interactionPositions[7]={3,1.25f,2};interactionPositions[8]={-3,1.15f,-3};interactionPositions[9]={3,1.15f,3};interactionPositions[10]={4.65f,1.5f,3.6f};interactionPositions[11]={3,1.25f,2};interactionPositions[12]={0,8.43f,-4.90f};
 LoadDesk();BuildWorld();LoadSettings();InputManager::GetInstance().SetCursorMode(GLFW_CURSOR_NORMAL);
}
void UltimoSinal::LoadDesk(){
 const char* path="Scenes/Models/Mesa/mesa.3ds";
 std::error_code error;
 if(!std::filesystem::is_regular_file(path,error)){
  std::cout<<"Mesa autoral indisponivel; mesa procedural provisoria mantida: "<<path<<std::endl;return;
 }
 auto candidate=std::make_unique<CAsset>(path,false);const auto& report=candidate->GetReport();
 const auto size=report.boundsMax-report.boundsMin;
 bool dimensions=report.hasBounds&&std::abs(size.x-1.6f)<.02f&&std::abs(size.y-1.f)<.02f&&std::abs(size.z-.9f)<.02f;
 bool attributes=report.positionsFinite&&report.normalsFinite&&report.uvsFinite&&report.meshCount>0
  &&report.meshesWithUVs==report.meshCount&&report.normalFallbackVertices==0&&report.uvFallbackVertices==0;
 bool valid=dimensions&&attributes&&report.triangleCount>=60&&report.triangleCount<20000&&report.diffuseTexturesLoaded>0&&report.error.empty();
 std::cout<<"Mesa 3DS: meshes="<<report.meshCount<<" triangles="<<report.triangleCount
  <<" size="<<size.x<<","<<size.y<<","<<size.z<<" diffuse="<<report.diffuseTexturesLoaded<<" accepted="<<valid<<std::endl;
 if(!valid){std::cerr<<"Mesa rejeitada para evitar escala/UV/material incorretos; fallback procedural ativo. "<<report.error<<std::endl;return;}
 // The 3DS importer and PreTransformVertices already convert Max Z-up to Y-up.
 // Recenter horizontally and place the measured feet on the existing floor.
 const glm::vec3 offset(-(report.boundsMin.x+report.boundsMax.x)*.5f,-report.boundsMin.y,-(report.boundsMin.z+report.boundsMax.z)*.5f);
 const std::array<glm::vec3,5> positions{{{-2.9f,.095f,1.8f},{-3,.095f,.25f},{-3,.095f,-3},{3,.095f,-3},{3,.095f,2.5f}}};
 desks.clear();desks.reserve(positions.size());
 for(std::size_t i=0;i<positions.size();++i){
  // Reuse the same imported mesh and UV material. The radio/letter workbench
  // needs a longer instance; the source Max/3DS model remains unmodified.
  const glm::vec3 scale(1,1,i==4?1.95f/size.z:1.f);
  const auto model=glm::translate(glm::mat4(1),positions[i])
   *glm::scale(glm::mat4(1),scale)*glm::translate(glm::mat4(1),offset);
  const auto center=((report.boundsMin+report.boundsMax)*.5f+offset)*scale+positions[i];
  const auto extent=size*scale;
  desks.push_back({model,{{center.x,center.y,center.z},{extent.x,extent.y,extent.z}}});
 }
 mesa=std::move(candidate);
 std::cout<<"Mobiliario Max aplicado: mesa_instances=4 escrivan_instances=1; collision_volumes="<<desks.size()<<std::endl;
}
UltimoSinal::~UltimoSinal(){Save();SaveSettings();if(evidenceDepth)glDeleteRenderbuffers(1,&evidenceDepth);if(evidenceTexture)glDeleteTextures(1,&evidenceTexture);if(evidenceFbo)glDeleteFramebuffers(1,&evidenceFbo);cube.cleanup();cylinder.cleanup();cone.cleanup();waterPlane.cleanup();}
void UltimoSinal::Start(){
 experience=ultimo::Experience{};inspectedEvidence=previewEvidence=-1;lensPlaybackTime=-1;
 investigation.Start();sessionActive=true;camera=CCamera({0,1.7f,17});camera.Zoom=58;answer.fill(0);target=-1;duration=0;worldTime=0;doorAmount=0;autosave=0;skipMouse=true;Save();ResetExperience(false);if(experience.introPending)investigation.screen=Screen::Intro;SyncCursor();
}
void UltimoSinal::SyncCursor(){bool desired=investigation.screen==Screen::Explore&&!benchmarkMode;if(captured!=desired){captured=desired;skipMouse=true;InputManager::GetInstance().SetCursorMode(captured?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);}}
bool UltimoSinal::Blocked(glm::vec3 p)const{
 if(std::abs(p.x)>12.5f||std::abs(p.z)>20.5f)return true;
 if(std::hypot(p.x,p.z+3.f)<.61f)return true;
 const float feet=p.y-ultimo::spatial::Eye;
 for(auto& b:blocks){const float lo=b.bounds.center.y-b.bounds.size.y*.5f,hi=b.bounds.center.y+b.bounds.size.y*.5f;
  if(b.solid&&hi>feet+.12f&&lo<p.y+.12f&&ultimo::BlocksWalker(p.x,p.z,b.bounds))return true;
 }
 if(p.y>8.5f)return std::hypot(p.x,p.z+3.f)>2.25f||std::hypot(p.x,p.z+3.f)<.79f;
 if(p.y>2.f&&std::hypot(p.x,p.z+3.f)>1.20f)return true;
 return p.y<4.4f&&ultimo::BlocksWalker(p.x,p.z,(!investigation.DoorOpen()||doorAmount<.9f)?door:OpenLeaf);
}
bool UltimoSinal::Visible(glm::vec3 p)const{
 ultimo::Vec3 a{camera.Position.x,camera.Position.y,camera.Position.z},b{p.x,p.y,p.z};
 for(auto& block:blocks)if(block.solid&&ultimo::SegmentHits(a,b,block.bounds))return false;
 return !ultimo::SegmentHits(a,b,(!investigation.DoorOpen()||doorAmount<.9f)?door:OpenLeaf);
}
void UltimoSinal::FindTarget(){
 target=-1;float best=1e6f;
 // Approximate each prop's visible footprint, with three degrees of aim grace.
 // Pick the best screen alignment inside that volume, then break ties by range.
 // Visibility still tests the physical walls/closed door; no through-wall hints.
 constexpr std::array<float,13> radius{{.32f,.24f,.60f,.16f,.28f,.29f,.60f,0.f,.29f,.29f,.50f,.44f,.40f}};
 for(int i=0;i<int(interactionPositions.size());++i){
  if(i==7||(i==3&&investigation.DoorOpen()))continue;
  const auto offset=interactionPositions[i]-camera.Position;const float distance=glm::length(offset);
  if(distance>=3.f||distance<=.05f)continue;
  const float along=glm::dot(offset,camera.Front);if(along<=0)continue;
  const float miss=glm::length(offset-camera.Front*along);
  if(miss>radius[i]+along*.0524f||!Visible(interactionPositions[i]))continue;
  const float score=miss/along+distance*.0005f;
  if(score<best){best=score;target=i;}
 }
}
void UltimoSinal::Interact(){
 if(target<0)return;
 if(target==10){investigation.screen=Screen::Panel;investigation.notice="Compare a ficha de manutenção com o estado do acesso ao cais.";}
 else if(target==11){investigation.screen=Screen::Radio;investigation.notice=investigation.powerSolved?"Escolha o canal de registros locais.":"O receptor não responde. Falta alimentação.";}
 else if(target==12){inspectedEvidence=-1;if(investigation.DoorOpen()&&camera.Position.y>8.5f){investigation.screen=Screen::LensConsole;investigation.notice="Compare os registros antes de preparar o ensaio local.";}else investigation.notice="O comando fica na sala de serviço restrita.";}
 else{inspectedEvidence=-1;bool keyBefore=investigation.DoorOpen();if(investigation.InspectEntry(target)){audio.Cue(keyBefore!=investigation.DoorOpen()?3:0);Save();}else audio.Cue(4);}
 toastTime=5;SyncCursor();
}
void UltimoSinal::ProcessSceneInput(GLFWwindow* window,float deltaTime){
 auto& in=InputManager::GetInstance();walking=false;
 if(!IsMenuBackdrop())RestorePlayerCamera();
 const bool timingActive=investigation.screen==Screen::Explore||investigation.screen==Screen::Reading||investigation.screen==Screen::Journal||investigation.screen==Screen::Panel||investigation.screen==Screen::Radio||investigation.screen==Screen::LensConsole||investigation.screen==Screen::Conclusion;
 if(glfwGetWindowAttrib(window,GLFW_FOCUSED)&&timingActive&&!investigation.submitted&&std::isfinite(deltaTime))duration+=(std::max)(0.f,deltaTime);
 if(!glfwGetWindowAttrib(window,GLFW_FOCUSED)){if(investigation.screen==Screen::Explore){investigation.TogglePause();Save();}SyncCursor();skipMouse=true;return;}
 if(remapping>=0){
  if(in.IsKeyJustPressed(GLFW_KEY_ESCAPE)){remapping=-1;return;}
  for(int k='A';k<='Z';++k)if(in.IsKeyJustPressed(k)){
   int* keys[]{&settings.forward,&settings.back,&settings.left,&settings.right,&settings.interact,&settings.journal};bool duplicate=false;for(int i=0;i<6;++i)if(i!=remapping&&*keys[i]==k)duplicate=true;
   if(duplicate)saveNotice="Essa tecla já tem uma ação. Escolha outra.";else{*keys[remapping]=k;remapping=-1;SaveSettings();saveNotice="Controle atualizado.";}return;
  }return;
 }
 if(ImGui::GetIO().WantTextInput){if(investigation.screen==Screen::Conclusion){autosave+=(std::max)(0.f,deltaTime);if(autosave>5){autosave=0;Save();}}return;}
 auto before=investigation.screen;
 if(before==Screen::Title){if(in.IsKeyJustPressed(GLFW_KEY_ENTER)&&!ImGui::GetIO().NavVisible){if(sessionActive){if(experience.introPending)FinishIntro();else{RestorePlayerCamera();investigation.screen=investigation.submitted?Screen::Complete:Screen::Explore;}}else if(hasSave)Load();else Start();}return;}
 if(before!=Screen::Help&&before!=Screen::Options&&before!=Screen::Controls&&before!=Screen::ConfirmNew&&in.IsKeyJustPressed(GLFW_KEY_F1)){menuReturn=int(before);investigation.screen=Screen::Help;SyncCursor();return;}
 if(before!=Screen::Help&&before!=Screen::Options&&before!=Screen::Controls&&before!=Screen::ConfirmNew&&in.IsKeyJustPressed(GLFW_KEY_F2)){menuReturn=int(before);investigation.screen=Screen::Options;SyncCursor();return;}
 if(in.IsKeyJustPressed(GLFW_KEY_ESCAPE)||in.IsKeyJustPressed(GLFW_KEY_TAB)){
  if(before==Screen::Reading){inspectedEvidence=-1;investigation.CloseReading();}
  else if(before==Screen::Journal||before==Screen::Panel||before==Screen::Radio||before==Screen::LensConsole||before==Screen::Conclusion)investigation.screen=Screen::Explore;
  else if(before==Screen::Options||before==Screen::Help||before==Screen::Controls||before==Screen::ConfirmNew){investigation.screen=Screen(menuReturn);SaveSettings();}
  else investigation.TogglePause();Save();SyncCursor();return;
 }
 if(in.IsKeyJustPressed(settings.journal))investigation.ToggleJournal();
 if(before==Screen::Reading&&in.IsKeyJustPressed(settings.interact)){inspectedEvidence=-1;investigation.CloseReading();}
 if(investigation.screen!=before){SyncCursor();return;}
 if(before==Screen::Intro){if(in.IsKeyJustPressed(GLFW_KEY_ENTER))FinishIntro();return;}
 if(before!=Screen::Explore)return;
 auto mouse=in.GetMouseDelta();camera.MouseSensitivity=settings.Sensitivity();if(skipMouse)skipMouse=false;else camera.ProcessMouseMovement(mouse.x,settings.invertY?-mouse.y:mouse.y);
 glm::vec3 forward=camera.Front;forward.y=0;forward=glm::normalize(forward);glm::vec3 right=camera.Right;right.y=0;right=glm::normalize(right);glm::vec3 move(0);
 if(in.IsKeyPressed(settings.forward))move+=forward;if(in.IsKeyPressed(settings.back))move-=forward;if(in.IsKeyPressed(settings.right))move+=right;if(in.IsKeyPressed(settings.left))move-=right;
 auto old=camera.Position;Walk(move,deltaTime);walking=glm::length(camera.Position-old)>.0001f;
 // Vertical position follows actual stair support, not a fixed eye height.
 // No forced vertical bob: it would overwrite the first stair's support height.
 FindTarget();if(in.IsKeyJustPressed(settings.interact))Interact();
 if(in.IsKeyJustPressed(GLFW_KEY_ENTER))investigation.BeginConclusion();
 autosave+=deltaTime;if(autosave>5){autosave=0;Save();}SyncCursor();
}
void UltimoSinal::Walk(glm::vec3 move,float dt){
 if(glm::length(move)<.001f)return;dt=(std::clamp)(dt,0.f,.1f);int steps=(std::max)(1,int(std::ceil(dt/.016f)));move=glm::normalize(move)*2.6f*dt/float(steps);
 for(int n=0;n<steps;++n){
  auto attempt=[&](glm::vec3 p){
   auto floor=ultimo::spatial::Support(p.x,p.z,camera.Position.y-ultimo::spatial::Eye);
   if(!floor||(*floor>ultimo::spatial::Ground+.23f&&!investigation.DoorOpen()))return;
   p.y=*floor+ultimo::spatial::Eye;if(!Blocked(p))camera.Position=p;
  };
  auto p=camera.Position;p.x+=move.x;attempt(p);p=camera.Position;p.z+=move.z;attempt(p);
 }
}
void UltimoSinal::DrawScene(float dt){
 const float playbackDelta=std::isfinite(dt)?(std::clamp)(dt,0.f,ultimo::LensPlaybackDuration):0.f;
 dt=ultimo::Experience::SafeDelta(dt);
 const bool focused=benchmarkMode||glfwGetWindowAttrib(glfwGetCurrentContext(),GLFW_FOCUSED);
 if(focused&&(investigation.screen==Screen::Explore||investigation.screen==Screen::Intro))worldTime+=dt;
 UpdateExperience(dt);
 float desired=investigation.DoorOpen()?1.f:0.f;
 if(focused&&investigation.screen==Screen::Explore){doorAmount+=(std::clamp)(desired-doorAmount,-dt*1.8f,dt*1.8f);toastTime=(std::max)(0.f,toastTime-dt);}
 const bool playbackActive=focused&&(investigation.screen==Screen::Explore||investigation.screen==Screen::LensConsole||investigation.screen==Screen::Panel||investigation.screen==Screen::Radio||investigation.screen==Screen::Reading||investigation.screen==Screen::Journal||investigation.screen==Screen::Conclusion||investigation.screen==Screen::Complete);
 audio.SetPlaybackActive(playbackActive);
 if(playbackActive&&lensPlaybackTime>=0){
  lensPlaybackTime+=playbackDelta;if(lensPlaybackTime>ultimo::LensPlaybackDuration)lensPlaybackTime=-1;
 }
 const auto radioOffset=interactionPositions[11]-camera.Position,panelOffset=interactionPositions[10]-camera.Position;
 auto pan=[&](glm::vec3 v){return glm::length(v)>.01f?glm::dot(glm::normalize(v),camera.Right):0.f;};
 audio.SetPerspective(glm::length(radioOffset),pan(radioOffset),glm::length(panelOffset),pan(panelOffset),investigation.powerSolved);
 bool inside=std::abs(camera.Position.x)<5.f&&camera.Position.z<5.f&&camera.Position.z> -6.f;audio.Update(dt,inside,walking&&investigation.screen==Screen::Explore,settings.master,settings.ambient,settings.effects);
 glClearColor(.13f,.18f,.20f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);DrawWorld();DrawInterface();DrawExperience();
}
