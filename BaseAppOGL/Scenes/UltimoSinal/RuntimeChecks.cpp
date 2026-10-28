#include "UltimoSinal.h"
#include "Storage.h"
#include <iostream>
#include <cmath>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <initializer_list>
bool UltimoSinal::VerifyRuntime(){
 benchmarkMode=true;int checks=0,failures=0;
 auto check=[&](bool ok,const char* text){++checks;std::cout<<(ok?"PASS ":"FAIL ")<<text<<std::endl;if(!ok)++failures;};
 LoadWorldAssets();
 if(std::filesystem::is_directory("Scenes/Models/Cenario")){
  const char* required[]={"ilha","costa","costalod","caminho","cais","carga","caisbox","anexo","piso","torre","porta","cadeira","caneca","armario","armporta","botas","escada","radio","quadro","botao","mapa","papel","relogio","arquivo","lumin","lampada","diario","alicate","marcas","chave","foto","agua","janvid","vidfar","led","fragment","radioluz","escrivan","decor"};
  check(worldAssets.size()==39,"biblioteca Max completa com os 39 modelos do cenario");
  for(const char* name:required){
   auto label=std::string("modelo Max arquivo presente: ")+name;
   check(std::filesystem::is_regular_file(std::string("Scenes/Models/Cenario/")+name+".3ds"),label.c_str());
   const auto found=worldAssets.find(name);label=std::string("modelo Max aceito sem fallback: ")+name;
   check(found!=worldAssets.end(),label.c_str());
   if(found!=worldAssets.end()){
    const auto& report=found->second->GetReport();
    const auto finite=[](glm::vec3 v){return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);};
    bool ordered=report.boundsMin.x<=report.boundsMax.x&&report.boundsMin.y<=report.boundsMax.y&&report.boundsMin.z<=report.boundsMax.z;
    label=std::string("modelo Max bounds normais UV e materiais validos: ")+name;
    check(report.error.empty()&&report.hasBounds&&finite(report.boundsMin)&&finite(report.boundsMax)&&ordered
     &&report.positionsFinite&&report.normalsFinite&&report.normalFallbackVertices==0
     &&report.uvsFinite&&report.meshesWithUVs==report.meshCount&&report.uvFallbackVertices==0
     &&report.vertexCount>0&&report.triangleCount>0&&report.missingTextures.empty(),label.c_str());
   }
  }
  const auto group=[&](std::initializer_list<const char*> names){return std::all_of(names.begin(),names.end(),[&](const char* name){return worldAssets.find(name)!=worldAssets.end();});};
  check(group({"ilha","costa","costalod","caminho","cais","caisbox"}),"cobertura Max do exterior e acesso ao cais");
  check(group({"anexo","piso","torre","escada"}),"cobertura Max das duas salas farol e escada");
  check(group({"carga","cadeira","caneca","armario","armporta","botas","arquivo"}),"cobertura Max do mobiliario e objetos ambientais");
  check(group({"diario","alicate","marcas","chave","foto","fragment","papel","mapa","relogio"}),"cobertura Max das cinco pistas e documentos");
  check(group({"porta","radio","quadro","botao","led","radioluz"}),"cobertura Max das interacoes e respostas de estado");
  check(group({"agua","janvid","vidfar","lumin","lampada"}),"cobertura Max de agua vidro e luminarias");
  const auto floor=worldAssets.find("piso");bool floorMap=false,floorAttributes=false;
  if(floor!=worldAssets.end()){
   const auto& report=floor->second->GetReport();
   floorMap=floorUsesCompleteMap&&report.diffuseTexturesLoaded>0&&report.missingTextures.empty()
    &&std::filesystem::is_regular_file("Scenes/Models/Cenario/p_bake.jpg")
    &&std::any_of(report.diffuseTexturePaths.begin(),report.diffuseTexturePaths.end(),[](const std::string& path){
     auto leaf=std::filesystem::path(path).filename().string();
     std::transform(leaf.begin(),leaf.end(),leaf.begin(),[](unsigned char c){return char(std::tolower(c));});
     return leaf=="p_bake.jpg";
    });
   const auto size=report.boundsMax-report.boundsMin;
   floorAttributes=report.hasBounds&&report.positionsFinite&&report.normalsFinite&&report.normalFallbackVertices==0
    &&report.uvsFinite&&report.meshesWithUVs==report.meshCount&&report.uvFallbackVertices==0
    &&std::abs(size.x-9.7f)<.01f&&std::abs(size.y-.04f)<.01f&&std::abs(size.z-10.7f)<.01f;
  }
  check(floorMap,"piso CompleteMap p_bake.jpg relacionado ao material importado");
  check(floorAttributes,"piso com bake preserva UV1 normais e dimensoes em metros");
  bool floorPlacement=false;
  if(floor!=worldAssets.end()){
   const auto& report=floor->second->GetReport();const glm::vec3 anchor(0,.075f,-.5f);
   const auto low=report.boundsMin+anchor,high=report.boundsMax+anchor;
   floorPlacement=std::abs(high.y-.095f)<.002f&&low.y>.05f&&high.y>.08f+.01f
    &&std::abs(low.z+5.85f)<.01f&&std::abs(high.z-4.85f)<.01f;
  }
  check(floorPlacement,"piso CompleteMap na altura .095 e acima da fundacao sem perder alinhamento das salas");
  const auto stair=worldAssets.find("escada");bool stairClear=false;
  if(stair!=worldAssets.end()){
   const auto& r=stair->second->GetReport();
   stairClear=r.boundsMin.x> -1.48f&&r.boundsMax.x<1.48f&&r.boundsMin.z> -4.5f&&r.boundsMax.z< -1.50f&&std::abs(r.boundsMin.y-.095f)<.01f&&r.boundsMax.y>7.30f;
  }
  check(stairClear,"escada helicoidal no eixo real da torre sem contato com paredes do anexo");
  check(!Blocked({-4.26f,1.7f,.65f}),"trecho decorativo antigo foi removido da parede oeste");
  check(!Blocked({-2.6f,1.7f,3.3f})&&!Blocked({-3.1f,1.7f,3.0f}),"area de uso e abertura do armario livre da antiga mesa");
 }else std::cout<<"Cenario Max ainda ausente: execucao de desenvolvimento com fallback, nao certifica producao completa."<<std::endl;
 const bool deskRequested=std::filesystem::is_regular_file("Scenes/Models/Mesa/mesa.3ds");
 check(!deskRequested||bool(mesa),"mesa 3DS presente deve carregar sem fallback");
 if(mesa){
  const auto& report=mesa->GetReport();const auto size=report.boundsMax-report.boundsMin;
  check(report.hasBounds&&std::abs(size.x-1.6f)<.02f&&std::abs(size.y-1.f)<.02f&&std::abs(size.z-.9f)<.02f,"mesa importada mede 1.6 x 1.0 x 0.9 m em Y-up");
  check(report.triangleCount>=60&&report.triangleCount<20000,"mesa autoral possui geometria valida; cinco pecas e acabamento comprovados no Max");
  check(report.positionsFinite&&report.normalsFinite&&report.normalFallbackVertices==0,"mesa tem posicoes e normais finitas sem fallback");
  check(report.uvsFinite&&report.meshesWithUVs==report.meshCount&&report.uvFallbackVertices==0,"mesa preserva UVs em todas as meshes");
  check(report.diffuseTexturesLoaded>0&&report.missingTextures.empty(),"bitmap difuso da mesa carregado por referencia do material");
  check(desks.size()==5,"cinco volumes de apoio: quatro mesas e uma escrivaninha");
  const std::array<glm::vec3,5> expected{{{-2.9f,.595f,1.8f},{-3,.595f,.25f},{-3,.595f,-3},{3,.595f,-3},{3,.595f,2.5f}}};
  bool supported=desks.size()==5,placements=supported,colliders=supported,matrices=supported;
  for(std::size_t i=0;i<desks.size()&&i<expected.size();++i){
   const auto& desk=desks[i];const auto low=glm::vec3(desk.model*glm::vec4(report.boundsMin,1));
   const auto high=glm::vec3(desk.model*glm::vec4(report.boundsMax,1));
   supported=supported&&std::abs(low.y-.095f)<.001f&&std::abs(high.y-1.095f)<.02f;
   const glm::vec3 center(desk.bounds.center.x,desk.bounds.center.y,desk.bounds.center.z);
   const glm::vec3 extent(desk.bounds.size.x,desk.bounds.size.y,desk.bounds.size.z);
   placements=placements&&glm::length(center-expected[i])<.02f&&std::abs(extent.z-(i==4?1.95f:.9f))<.02f;
   matrices=matrices&&glm::length((low+high)*.5f-center)<.001f&&glm::length((high-low)-extent)<.001f;
   colliders=colliders&&Blocked({center.x,1.7f,center.z});
  }
  check(placements,"mesas Max na primeira e segunda sala; bancada de radio com profundidade 1.95m");
  check(supported,"cinco mesas apoiadas no piso e tampos na altura prevista");
  check(matrices,"volumes de mobiliario coincidem com as dimensoes previstas");
  check(colliders&&!Blocked({1.7f,1.7f,-3}),"cinco colisoes das mesas importadas preservam acesso a fotografia");
 }else std::cout<<"Mesa: fallback procedural; exportacao autoral ainda ausente, nao certificada."<<std::endl;
 auto persist=[&](const char* label){auto expected=investigation;benchmarkMode=false;Save();bool ok=Load();benchmarkMode=true;check(ok&&investigation.discovered==expected.discovered&&investigation.powerSolved==expected.powerSolved&&investigation.radioSolved==expected.radioSolved&&investigation.submitted==expected.submitted&&investigation.lensSolved==expected.lensSolved&&investigation.lensManual==expected.lensManual&&investigation.lensLong==expected.lensLong,label);};
 Start();persist("salvar e continuar no inicio");check(Blocked({0,1.7f,-.5f}),"porta trancada");check(!Visible(interactionPositions[4]),"pista atras da porta oculta");
 FindTarget();check(target!=12,"comando da sala optica nao e acessivel do cais");
 auto travel=[&](glm::vec3 destination){for(int n=0;n<1800&&glm::length(destination-camera.Position)>.025f;++n){auto v=destination-camera.Position;v.y=0;Walk(v,(std::min)(1.f/60.f,glm::length(v)/2.6f));}if(glm::length(destination-camera.Position)>=.035f)std::cout<<"TRAVEL from "<<camera.Position.x<<","<<camera.Position.y<<","<<camera.Position.z<<" to "<<destination.x<<","<<destination.y<<","<<destination.z<<" door="<<investigation.DoorOpen()<<" blocked="<<Blocked(destination)<<std::endl;check(glm::length(destination-camera.Position)<.035f,"percurso com colisoes");};
 auto examine=[&](int id,glm::vec3 pose){travel(pose);auto d=glm::normalize(interactionPositions[id]-camera.Position);camera=CCamera(camera.Position,{0,1,0},glm::degrees(std::atan2(d.z,d.x)),glm::degrees(std::asin(d.y)));FindTarget();check(target==id,"alvo real acessivel sem atravessar paredes");if(target==id)Interact();investigation.screen=ultimo::Screen::Explore;};
 examine(2,{-2,1.7f,11});travel({0,1.7f,6.2f});travel({0,1.7f,3.5f});
 examine(6,{1.7f,1.7f,3.3f});travel({0,1.7f,2});
 examine(5,{-1.4f,1.7f,.25f});examine(1,{-1.7f,1.7f,.25f});examine(0,{-1.7f,1.7f,1.8f});
 travel({0,1.7f,3.5f});examine(10,{3.9f,1.7f,3.8f});
 investigation.screen=ultimo::Screen::Panel;investigation.breakers={false,true,true};check(investigation.RestorePower(),"puzzle eletrico resolvido com evidencias");investigation.screen=ultimo::Screen::Explore;persist("salvar e continuar apos energia");
 // A title-menu visit has no loaded session: destructor/shortcut saves must leave
 // the existing on-disk investigation byte-for-byte intact in every submenu.
 {
  wchar_t isolatedSave[32768]{};GetEnvironmentVariableW(L"ULTIMO_TEST_HOME",isolatedSave,32768);
  const auto progress=std::filesystem::path(isolatedSave)/L"progress.v3";
  const auto readBytes=[&](){std::ifstream input(progress,std::ios::binary);return std::string((std::istreambuf_iterator<char>(input)),{});};
  const auto original=readBytes();const auto activeInvestigation=investigation;
  const bool activeSession=sessionActive,activeBenchmark=benchmarkMode;
  investigation=ultimo::Investigation{};sessionActive=false;benchmarkMode=false;
  for(auto screen:{ultimo::Screen::Options,ultimo::Screen::Controls,ultimo::Screen::Help}){
   investigation.screen=screen;Save();
   check(!original.empty()&&readBytes()==original,screen==ultimo::Screen::Options?"opcoes antes de continuar preservam save":screen==ultimo::Screen::Controls?"controles antes de continuar preservam save":"ajuda antes de continuar preserva save");
  }
  investigation=activeInvestigation;sessionActive=activeSession;benchmarkMode=activeBenchmark;
 }
 travel({0,1.7f,3.5f});examine(3,{-2.6f,1.7f,3.8f});doorAmount=1;check(!Blocked({0,1.7f,-.5f}),"chave abre passagem");check(Blocked({-1.15f,1.7f,.50f}),"folha aberta continua solida");
 investigation.screen=ultimo::Screen::LensConsole;investigation.lensManual=true;investigation.lensLong={true,true,true};
 check(!investigation.VerifyLens()&&!investigation.lensSolved,"console recusa sequencia correta sem radio fotografia e registro");
 investigation.lensManual=false;investigation.lensLong={false,false,false};investigation.screen=ultimo::Screen::Explore;
 travel({0,1.7f,2});examine(11,{1.7f,1.7f,2});investigation.screen=ultimo::Screen::Radio;investigation.radioChannel=6;check(investigation.TuneRadio(),"radio e transcricao");investigation.screen=ultimo::Screen::Explore;
 travel({0,1.7f,2});travel({0,1.7f,-1.2f});travel({-1.8f,1.7f,-1.3f});examine(8,{-1.7f,1.7f,-3});travel({-1.8f,1.7f,-1.3f});travel({1.8f,1.7f,-1.3f});examine(4,{1.7f,1.7f,-3});
 check(!investigation.Ready()&&!investigation.BeginConclusion(),"documentos reunidos ainda exigem reconstituicao local antes do relatorio");
 const auto reachLensConsole=[&](){
  if(camera.Position.y<2){
   travel({1.8f,1.7f,-1.3f});travel({1.7f,1.7f,-2.8f});
   for(int j=0;j<36;++j){float angle=glm::radians(10.f+j*20.f);travel({1.06f*std::cos(angle),1.9f+j*.20f,-3.f+1.06f*std::sin(angle)});}
   travel({1.65f,8.9f,-3.15f});travel({1.75f,8.9f,-4.0f});travel({1.0f,8.9f,-4.5f});
  }
  check(ultimo::spatial::Phase(camera.Position.z,camera.Position.y)==3,"subida real de 36 degraus leva a terceira etapa na sala optica");
  auto d=glm::normalize(interactionPositions[12]-camera.Position);
  camera=CCamera(camera.Position,{0,1,0},glm::degrees(std::atan2(d.z,d.x)),glm::degrees(std::asin(d.y)));
  FindTarget();check(target==12,"comando fisico acessivel somente na sala optica elevada");
  if(target==12)Interact();check(investigation.screen==ultimo::Screen::LensConsole,"console da lente abre na terceira etapa");
 };
 reachLensConsole();
 investigation.lensManual=false;investigation.lensLong={true,true,true};
 check(!investigation.VerifyLens()&&!investigation.lensSolved,"modo automatico recusa reconstituicao mesmo com tres pulsos longos");
 investigation.lensManual=true;investigation.lensLong={true,false,true};
 check(!investigation.VerifyLens()&&!investigation.lensSolved,"pulso curto recusa reconstituicao mesmo em comando manual");
 persist("salvar e continuar preserva rascunho manual longo curto longo sem resolver");
 reachLensConsole();investigation.lensLong={true,true,true};
 check(investigation.VerifyLens()&&investigation.lensSolved,"comando manual e tres pulsos longos conferem reconstituicao local");
 persist("salvar e continuar preserva reconstituicao conferida e seus controles");
 investigation.screen=ultimo::Screen::Explore;
 travel({1.75f,8.9f,-4.0f});travel({1.65f,8.9f,-3.15f});
 for(int j=35;j>=0;--j){float angle=glm::radians(10.f+j*20.f);travel({1.06f*std::cos(angle),1.9f+j*.20f,-3.f+1.06f*std::sin(angle)});}
 travel({1.7f,1.7f,-2.8f});
 check(std::abs(camera.Position.y-1.7f)<.01f,"descida completa devolve jogador ao arquivo sem queda nem teletransporte");
 check(investigation.BeginConclusion(),"relacoes completas abrem relatorio");investigation.hypothesis=1;investigation.evidence[7]=investigation.evidence[8]=true;investigation.justification="Teste tecnico: tempos e comando manual, sem participante.";check(investigation.Submit()&&investigation.Correct(),"desfecho e resposta");persist("salvar e continuar desfecho");
 ultimo::Snapshot snap;snap.game=investigation;snap.complete=true;snap.x=camera.Position.x;snap.z=camera.Position.z;ultimo::Snapshot reload;check(ultimo::Decode(ultimo::Encode(snap),reload)&&reload.game.submitted,"desfecho recuperavel");
 wchar_t isolated[32768]{};GetEnvironmentVariableW(L"ULTIMO_TEST_HOME",isolated,32768);auto file=std::filesystem::path(isolated)/L"progress.v3";
 {std::ifstream input(file,std::ios::binary);std::string original((std::istreambuf_iterator<char>(input)),{});input.close();{std::ofstream bad(file,std::ios::binary|std::ios::app);bad<<"corrupted";}bool rejected=!Load();check(rejected&&investigation.submitted,"save corrompido rejeitado sem apagar sessao");{std::ofstream restore(file,std::ios::binary);restore<<original;}}
 settings.master=0;settings.sensitivity=.10f;settings.brightness=1.2f;settings.reduceMotion=true;SaveSettings();settings=ultimo::Settings{};LoadSettings();check(settings.master==0&&settings.reduceMotion&&std::abs(settings.brightness-1.2f)<.001f&&settings.sensitivity==.1f,"opcoes persistem em disco");settings.Reset();
 Start();check(!investigation.DoorOpen()&&!investigation.powerSolved&&!investigation.radioSolved&&!investigation.lensSolved&&!investigation.lensManual&&std::none_of(investigation.lensLong.begin(),investigation.lensLong.end(),[](bool value){return value;}),"reinicio limpa mecanismos e rascunho do console da lente");
 // Exercise the real presentation/save boundary with the isolated test slot.
 benchmarkMode=false;Start();
 check(investigation.screen==ultimo::Screen::Intro&&experience.introPending,"nova partida apresenta prologo antes de liberar exploracao");
 auto bytes=[&](){std::ifstream in(file,std::ios::binary);return std::string((std::istreambuf_iterator<char>(in)),{});};
 const auto beforeIntro=bytes();camera.Position={0,2.3f,18.5f};Save();
 check(!beforeIntro.empty()&&bytes()==beforeIntro,"camera cinematica do prologo nao sobrescreve posicao salva");
 FinishIntro();
 check(!experience.introPending&&investigation.screen==ultimo::Screen::Explore&&glm::length(camera.Position-glm::vec3(0,1.7f,17))<.001f,"pular introducao devolve camera e controle ao jogador");
 const auto beforeMenu=bytes();const auto player=camera;
 experience.heldPlayerPose={player.Position.x,player.Position.y,player.Position.z,player.Yaw,player.Pitch,player.Zoom};experience.menuCameraHeld=true;
 camera=CCamera({6.6f,2.8f,13.45f});investigation.screen=ultimo::Screen::Options;menuReturn=int(ultimo::Screen::Title);Save();
 check(bytes()==beforeMenu,"opcoes sobre menu cinematico preservam salvamento do jogador");
 RestorePlayerCamera();
 check(!experience.menuCameraHeld&&glm::length(camera.Position-player.Position)<.001f&&camera.Yaw==player.Yaw,"continuar restaura posicao e orientacao anteriores ao menu");
 sessionActive=false;investigation.screen=ultimo::Screen::Pause;investigation.discovered[3]=true;doorAmount=0;
 const auto pausedClock=worldTime;const auto pausedCabinet=experience.cabinetAmount;
 ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();ImGui::NewFrame();DrawScene(.1f);ImGui::Render();
 check(doorAmount==0&&worldTime==pausedClock&&experience.cabinetAmount==pausedCabinet,"pausa congela porta armario e relogio do mundo");
 check(glGetError()==GL_NO_ERROR,"menu de pausa da nova apresentacao desenha sem erro GL");
 benchmarkMode=true;
 {
  investigation.screen=ultimo::Screen::LensConsole;investigation.lensSolved=true;
  lensPlaybackTime=-1;StartLensPlayback();lensPlaybackTime=.43f;StartLensPlayback();
  check(lensPlaybackTime==.43f,"reproducao ativa nao reinicia nem sobrepoe sinal local");
  lensPlaybackTime=-1;
  camera=CCamera({3.9f,1.7f,3.8f});auto direction=glm::normalize(interactionPositions[10]-camera.Position);
  camera=CCamera(camera.Position,{0,1,0},glm::degrees(std::atan2(direction.z,direction.x))+7.f,glm::degrees(std::asin(direction.y)));
  FindTarget();check(target==10,"volume do painel tolera mira deslocada sem exigir ponto central");
 }
 // Exercise the actual imported meshes and the thumbnail renderer, including
 // hostile caller state, rather than merely testing that a texture ID exists.
 {
  GLint program,viewport[4],drawFbo,readFbo,active;
  glGetIntegerv(GL_CURRENT_PROGRAM,&program);glGetIntegerv(GL_VIEWPORT,viewport);
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&drawFbo);glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&readFbo);glGetIntegerv(GL_ACTIVE_TEXTURE,&active);
  const bool oldScissor=glIsEnabled(GL_SCISSOR_TEST);GLboolean mask[4];glGetBooleanv(GL_COLOR_WRITEMASK,mask);
  glEnable(GL_SCISSOR_TEST);glColorMask(GL_TRUE,GL_FALSE,GL_TRUE,GL_FALSE);
  evidenceYaw=0;evidencePitch=0;evidenceZoom=1;
  auto pixels=[&](){
   std::vector<unsigned char> data(640*320*3);GLint previous;glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&previous);
   glBindFramebuffer(GL_READ_FRAMEBUFFER,evidenceFbo);glReadPixels(0,0,640,320,GL_RGB,GL_UNSIGNED_BYTE,data.data());glBindFramebuffer(GL_READ_FRAMEBUFFER,previous);return data;
  };
  for(int id:{0,1,3,4}){
   const bool rendered=RenderEvidencePreview(id);check(rendered,"inspecao usa modelo Max nativo de diario alicate chave e foto");
   if(rendered){auto data=pixels();std::size_t varied=0;for(std::size_t p=3;p<data.size();p+=3)if(std::abs(int(data[p])-int(data[0]))+std::abs(int(data[p+1])-int(data[1]))+std::abs(int(data[p+2])-int(data[2]))>20)++varied;
    check(varied>150,"inspecao produz imagem do objeto em vez de textura vazia");}
  }
  RenderEvidencePreview(1);const auto first=pixels();evidenceYaw=65;RenderEvidencePreview(1);const auto rotated=pixels();
  std::size_t changed=0;for(std::size_t i=0;i<first.size();++i)if(std::abs(int(first[i])-int(rotated[i]))>12)++changed;
  check(changed>150,"girar inspecao modifica a geometria visivel");
  GLint nowProgram,nowViewport[4],nowDraw,nowRead,nowActive;GLboolean nowMask[4];
  glGetIntegerv(GL_CURRENT_PROGRAM,&nowProgram);glGetIntegerv(GL_VIEWPORT,nowViewport);glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&nowDraw);glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&nowRead);glGetIntegerv(GL_ACTIVE_TEXTURE,&nowActive);glGetBooleanv(GL_COLOR_WRITEMASK,nowMask);
  check(nowProgram==program&&nowDraw==drawFbo&&nowRead==readFbo&&nowActive==active&&std::equal(viewport,viewport+4,nowViewport)&&glIsEnabled(GL_SCISSOR_TEST)&&nowMask[0]&&!nowMask[1]&&nowMask[2]&&!nowMask[3],"inspecao restaura framebuffer viewport shader textura scissor e mascara");
  check(!RenderEvidencePreview(2),"vestigio ambiental nao cria inspecao de objeto inexistente");
  if(!oldScissor)glDisable(GL_SCISSOR_TEST);glColorMask(mask[0],mask[1],mask[2],mask[3]);evidenceYaw=15;evidencePitch=0;evidenceZoom=1;
  check(glGetError()==GL_NO_ERROR,"inspecoes nativas nao deixam erros OpenGL");
 }
 std::cout<<"Runtime: "<<checks<<" checks; failures="<<failures<<std::endl;return failures==0;
}
void UltimoSinal::BenchmarkPose(float t){benchmarkMode=true;investigation.screen=ultimo::Screen::Explore;investigation.powerSolved=true;investigation.discovered[3]=true;doorAmount=1;worldTime=t;float phase=std::fmod(t,40.f)/40.f;camera=CCamera({0,1.7f,17.f-20.f*(1.f-std::abs(2.f*phase-1.f))},{0,1,0},-90.f+50.f*std::sin(t*.3f),-5.f);camera.Zoom=58;}
