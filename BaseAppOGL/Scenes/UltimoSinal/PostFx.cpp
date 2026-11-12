#include "UltimoSinal.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

using ultimo::Screen;

// Optional screen-space finish for scene 2: the world is drawn into a linear
// HDR target, then ambient occlusion, bloom and a single display transform are
// applied. Every failure path (shader file missing, compile or link error,
// incomplete framebuffer) disables these passes for the session and the world
// is drawn directly, exactly as it was before this file existed. The "Baixa"
// quality preset never uses them. Engine sources are not touched.
namespace {
std::string ReadText(const char* path){
 std::ifstream file(path,std::ios::binary);if(!file)return {};
 std::ostringstream text;text<<file.rdbuf();return text.str();
}
GLuint Compile(GLenum type,const std::string& text){
 if(text.empty())return 0;
 const GLuint shader=glCreateShader(type);const char* source=text.c_str();
 glShaderSource(shader,1,&source,nullptr);glCompileShader(shader);
 GLint ok=GL_FALSE;glGetShaderiv(shader,GL_COMPILE_STATUS,&ok);
 if(ok!=GL_TRUE){
  char log[1024]{};glGetShaderInfoLog(shader,GLsizei(sizeof(log)),nullptr,log);
  std::cerr<<"PostFx shader: "<<log<<std::endl;glDeleteShader(shader);return 0;
 }
 return shader;
}
GLuint Link(){
 const GLuint vertex=Compile(GL_VERTEX_SHADER,ReadText("Scenes/UltimoSinal/Post.vert"));
 const GLuint fragment=Compile(GL_FRAGMENT_SHADER,ReadText("Scenes/UltimoSinal/Post.frag"));
 GLuint program=0;
 if(vertex&&fragment){
  program=glCreateProgram();glAttachShader(program,vertex);glAttachShader(program,fragment);glLinkProgram(program);
  GLint ok=GL_FALSE;glGetProgramiv(program,GL_LINK_STATUS,&ok);
  if(ok!=GL_TRUE){
   char log[1024]{};glGetProgramInfoLog(program,GLsizei(sizeof(log)),nullptr,log);
   std::cerr<<"PostFx link: "<<log<<std::endl;glDeleteProgram(program);program=0;
  }
 }
 if(vertex)glDeleteShader(vertex);
 if(fragment)glDeleteShader(fragment);
 return program;
}
GLuint Target(GLenum internalFormat,GLenum format,GLenum type,int width,int height,GLint filter){
 GLuint id=0;glGenTextures(1,&id);glBindTexture(GL_TEXTURE_2D,id);
 glTexImage2D(GL_TEXTURE_2D,0,GLint(internalFormat),width,height,0,format,type,nullptr);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,filter);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,filter);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
 return id;
}
bool Complete(){return glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE;}
void Integer(GLuint program,const char* name,int value){glUniform1i(glGetUniformLocation(program,name),value);}
void Scalar(GLuint program,const char* name,float value){glUniform1f(glGetUniformLocation(program,name),value);}
void Pair(GLuint program,const char* name,float x,float y){glUniform2f(glGetUniformLocation(program,name),x,y);}
void Triple(GLuint program,const char* name,glm::vec3 value){glUniform3f(glGetUniformLocation(program,name),value.x,value.y,value.z);}
// Per-area colour treatment: shadow tint, highlight tint and saturation.
struct Grade{glm::vec3 shadow,highlight;float saturation;};
Grade GradeFor(ultimo::AcousticArea area){
 switch(area){
 case ultimo::AcousticArea::Service:return {{.95f,.99f,1.05f},{1.05f,1.00f,.93f},.90f};
 case ultimo::AcousticArea::Archive:return {{.93f,.99f,1.08f},{.98f,1.01f,1.04f},.88f};
 case ultimo::AcousticArea::Stair:return {{.93f,1.00f,1.07f},{1.02f,1.00f,.96f},.92f};
 case ultimo::AcousticArea::Optics:return {{.94f,1.00f,1.06f},{1.05f,1.00f,.92f},.96f};
 default:return {{.91f,.98f,1.08f},{1.03f,1.00f,.95f},.93f};
 }
}
}

void UltimoSinal::ReleasePost(bool keepProgram){
 const auto texture=[](GLuint& id){if(id){glDeleteTextures(1,&id);id=0;}};
 const auto buffer=[](GLuint& id){if(id){glDeleteRenderbuffers(1,&id);id=0;}};
 const auto framebuffer=[](GLuint& id){if(id){glDeleteFramebuffers(1,&id);id=0;}};
 if(post.sceneFbo==post.resolveFbo)post.sceneFbo=0;
 framebuffer(post.sceneFbo);framebuffer(post.resolveFbo);buffer(post.sceneColor);buffer(post.sceneDepth);
 texture(post.resolveColor);texture(post.resolveDepth);framebuffer(post.occlusionFbo);texture(post.occlusionTexture);
 for(auto& id:post.bloomFbo)framebuffer(id);
 for(auto& id:post.bloomTexture)texture(id);
 post.width=post.height=post.samples=0;post.active=false;
 if(!keepProgram){
  if(post.program){glDeleteProgram(post.program);post.program=0;}
  if(post.vao){glDeleteVertexArrays(1,&post.vao);post.vao=0;}
 }
}

bool UltimoSinal::PreparePost(int width,int height){
 if(post.failed)return false;
 if(!post.program){
  post.program=Link();
  if(!post.program){post.failed=true;std::cerr<<"PostFx indisponivel: a cena segue no desenho direto."<<std::endl;return false;}
  glGenVertexArrays(1,&post.vao);
 }
 if(post.resolveFbo&&post.width==width&&post.height==height)return true;
 ReleasePost(true);
 GLint oldActive=GL_TEXTURE0,oldTexture=0,oldRenderbuffer=0,oldDraw=0,oldRead=0;
 glGetIntegerv(GL_ACTIVE_TEXTURE,&oldActive);glActiveTexture(GL_TEXTURE0);glGetIntegerv(GL_TEXTURE_BINDING_2D,&oldTexture);
 glGetIntegerv(GL_RENDERBUFFER_BINDING,&oldRenderbuffer);
 glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&oldDraw);glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&oldRead);

 // Single-sample colour and depth: inputs of the occlusion, bloom and composite.
 post.resolveColor=Target(GL_RGBA16F,GL_RGBA,GL_FLOAT,width,height,GL_LINEAR);
 post.resolveDepth=Target(GL_DEPTH_COMPONENT24,GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,width,height,GL_NEAREST);
 glGenFramebuffers(1,&post.resolveFbo);glBindFramebuffer(GL_FRAMEBUFFER,post.resolveFbo);
 glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,post.resolveColor,0);
 glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,post.resolveDepth,0);
 bool ok=Complete();
 // The world keeps the 4x multisampling of the window when the driver allows
 // a floating-point multisample target; otherwise it draws into the textures.
 GLint maxSamples=0;glGetIntegerv(GL_MAX_SAMPLES,&maxSamples);
 if(ok&&maxSamples>=4){
  glGenRenderbuffers(1,&post.sceneColor);glBindRenderbuffer(GL_RENDERBUFFER,post.sceneColor);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER,4,GL_RGBA16F,width,height);
  glGenRenderbuffers(1,&post.sceneDepth);glBindRenderbuffer(GL_RENDERBUFFER,post.sceneDepth);
  glRenderbufferStorageMultisample(GL_RENDERBUFFER,4,GL_DEPTH_COMPONENT24,width,height);
  glGenFramebuffers(1,&post.sceneFbo);glBindFramebuffer(GL_FRAMEBUFFER,post.sceneFbo);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,post.sceneColor);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,post.sceneDepth);
  if(Complete())post.samples=4;
  else{
   glDeleteFramebuffers(1,&post.sceneFbo);post.sceneFbo=0;
   glDeleteRenderbuffers(1,&post.sceneColor);post.sceneColor=0;
   glDeleteRenderbuffers(1,&post.sceneDepth);post.sceneDepth=0;
  }
 }
 if(ok&&post.samples==0)post.sceneFbo=post.resolveFbo;

 post.occlusionTexture=Target(GL_R8,GL_RED,GL_UNSIGNED_BYTE,width,height,GL_LINEAR);
 glGenFramebuffers(1,&post.occlusionFbo);glBindFramebuffer(GL_FRAMEBUFFER,post.occlusionFbo);
 glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,post.occlusionTexture,0);
 ok=ok&&Complete();
 int levelWidth=width,levelHeight=height;
 for(std::size_t i=0;i<post.bloomFbo.size();++i){
  levelWidth=(std::max)(1,levelWidth/2);levelHeight=(std::max)(1,levelHeight/2);
  post.bloomWidth[i]=levelWidth;post.bloomHeight[i]=levelHeight;
  post.bloomTexture[i]=Target(GL_R11F_G11F_B10F,GL_RGB,GL_FLOAT,levelWidth,levelHeight,GL_LINEAR);
  glGenFramebuffers(1,&post.bloomFbo[i]);glBindFramebuffer(GL_FRAMEBUFFER,post.bloomFbo[i]);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,post.bloomTexture[i],0);
  ok=ok&&Complete();
 }
 glBindFramebuffer(GL_DRAW_FRAMEBUFFER,GLuint(oldDraw));glBindFramebuffer(GL_READ_FRAMEBUFFER,GLuint(oldRead));
 glBindRenderbuffer(GL_RENDERBUFFER,GLuint(oldRenderbuffer));
 glBindTexture(GL_TEXTURE_2D,GLuint(oldTexture));glActiveTexture(GLenum(oldActive));
 if(!ok){
  ReleasePost(true);post.failed=true;
  std::cerr<<"PostFx: alvo de render incompleto; a cena segue no desenho direto."<<std::endl;return false;
 }
 post.width=width;post.height=height;
 std::cout<<"PostFx: "<<width<<"x"<<height<<" HDR, amostras="<<post.samples<<std::endl;
 return true;
}

bool UltimoSinal::BeginPost(){
 post.active=false;
 if(!postEnabled||post.failed||(std::clamp)(settings.quality,0,2)==0)return false;
 GLint viewport[4]{};glGetIntegerv(GL_VIEWPORT,viewport);
 const int width=viewport[2],height=viewport[3];
 if(viewport[0]!=0||viewport[1]!=0||width<16||height<16||width>8192||height>8192)return false;
 if(!PreparePost(width,height))return false;
 glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&post.previousDraw);glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&post.previousRead);
 glBindFramebuffer(GL_FRAMEBUFFER,post.sceneFbo);
 glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 post.active=true;return true;
}

void UltimoSinal::EndPost(){
 if(!post.active)return;
 post.active=false;
 const int width=post.width,height=post.height;
 const int quality=(std::clamp)(settings.quality,0,2);
 GLint oldActive=GL_TEXTURE0;glGetIntegerv(GL_ACTIVE_TEXTURE,&oldActive);
 if(post.sceneFbo!=post.resolveFbo){
  glBindFramebuffer(GL_READ_FRAMEBUFFER,post.sceneFbo);glBindFramebuffer(GL_DRAW_FRAMEBUFFER,post.resolveFbo);
  glBlitFramebuffer(0,0,width,height,0,0,width,height,GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT,GL_NEAREST);
 }
 glDisable(GL_DEPTH_TEST);glDisable(GL_BLEND);glDisable(GL_CULL_FACE);glDepthMask(GL_FALSE);
 glUseProgram(post.program);glBindVertexArray(post.vao);
 const GLuint program=post.program;
 Integer(program,"source",0);Integer(program,"depthMap",1);Integer(program,"occlusionMap",2);Integer(program,"bloomMap",3);
 Pair(program,"projection",lastProjection[0][0],lastProjection[1][1]);Pair(program,"depthRange",.1f,240.f);
 Scalar(program,"aspect",float(width)/float(height));
 glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D,post.resolveDepth);

 const bool occlusion=aoEnabled;
 if(occlusion){
  glBindFramebuffer(GL_FRAMEBUFFER,post.occlusionFbo);glViewport(0,0,width,height);
  Integer(program,"mode",2);Integer(program,"occlusionSamples",quality==2?16:9);
  Pair(program,"texel",1.f/float(width),1.f/float(height));
  glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,post.resolveColor);
  glDrawArrays(GL_TRIANGLES,0,3);
 }
 const bool bloom=bloomEnabled;
 if(bloom){
  const int levels=int(post.bloomFbo.size());
  Integer(program,"mode",0);glActiveTexture(GL_TEXTURE0);
  for(int i=0;i<levels;++i){
   const int sourceWidth=i==0?width:post.bloomWidth[i-1],sourceHeight=i==0?height:post.bloomHeight[i-1];
   glBindFramebuffer(GL_FRAMEBUFFER,post.bloomFbo[i]);glViewport(0,0,post.bloomWidth[i],post.bloomHeight[i]);
   glBindTexture(GL_TEXTURE_2D,i==0?post.resolveColor:post.bloomTexture[i-1]);
   Scalar(program,"prefilter",i==0?1.f:0.f);Pair(program,"texel",1.f/float(sourceWidth),1.f/float(sourceHeight));
   glDrawArrays(GL_TRIANGLES,0,3);
  }
  // Each wider level is added back onto the sharper one above it.
  Integer(program,"mode",1);glEnable(GL_BLEND);glBlendFunc(GL_ONE,GL_ONE);
  for(int i=levels-2;i>=0;--i){
   glBindFramebuffer(GL_FRAMEBUFFER,post.bloomFbo[i]);glViewport(0,0,post.bloomWidth[i],post.bloomHeight[i]);
   glBindTexture(GL_TEXTURE_2D,post.bloomTexture[i+1]);
   Pair(program,"texel",1.f/float(post.bloomWidth[i+1]),1.f/float(post.bloomHeight[i+1]));
   glDrawArrays(GL_TRIANGLES,0,3);
  }
  glDisable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
 }

 glBindFramebuffer(GL_DRAW_FRAMEBUFFER,GLuint(post.previousDraw));glBindFramebuffer(GL_READ_FRAMEBUFFER,GLuint(post.previousRead));
 glViewport(0,0,width,height);
 Integer(program,"mode",3);Pair(program,"texel",1.f/float(width),1.f/float(height));
 glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,post.resolveColor);
 glActiveTexture(GL_TEXTURE2);glBindTexture(GL_TEXTURE_2D,post.occlusionTexture);
 glActiveTexture(GL_TEXTURE3);glBindTexture(GL_TEXTURE_2D,post.bloomTexture[0]);
 const bool cinematic=IsMenuBackdrop()||investigation.screen==Screen::Intro;
 const bool opticalRoom=camera.Position.y>8.2f&&std::hypot(camera.Position.x,camera.Position.z+3.f)<2.6f;
 Scalar(program,"occlusionStrength",occlusion?(quality==2?.90f:.80f):0.f);
 Scalar(program,"bloomStrength",bloom?.34f:0.f);
 Scalar(program,"exposure",1.f+.10f*drama.storm);
 Scalar(program,"saturation",gradeSaturation);Scalar(program,"contrast",.18f);
 Triple(program,"shadowTint",gradeShadow);Triple(program,"highlightTint",gradeHighlight);
 Triple(program,"veil",glm::vec3(.020f,.027f,.042f)*drama.storm+glm::vec3(.050f,.031f,.010f)*(opticalRoom?drama.lensFlash:0.f));
 Scalar(program,"vignette",cinematic?.46f:.30f);
 Scalar(program,"grain",.022f);
 Scalar(program,"grainSeed",settings.reduceMotion||benchmarkMode?0.f:std::fmod(worldTime+experience.menuTime,1.f)*61.f);
 glDrawArrays(GL_TRIANGLES,0,3);

 // Leave the state DrawWorld leaves: the interface is drawn next.
 // DrawWorld rebinds its own units 0..3 at the start of every frame.
 for(int unit=3;unit>=0;--unit){glActiveTexture(GLenum(GL_TEXTURE0+unit));glBindTexture(GL_TEXTURE_2D,0);}
 glActiveTexture(GLenum(oldActive));
 glBindVertexArray(0);glUseProgram(0);
 glDepthMask(GL_TRUE);glEnable(GL_DEPTH_TEST);
}

void UltimoSinal::RenderWorld(){
 // Stair smoothing, the walking sway and the running field of view are view
 // adjustments only. Collision, targeting and the saved pose keep the logical
 // camera, which is restored before any other system reads it.
 if(benchmarkMode)UpdateViewFeel(0.f);
 const float logicalY=camera.Position.y,logicalZoom=camera.Zoom;
 camera.Position.y+=renderLift;camera.Zoom+=renderZoom;
 const bool hdr=BeginPost();
 DrawWorld();
 if(hdr)EndPost();
 camera.Position.y=logicalY;camera.Zoom=logicalZoom;
}

void UltimoSinal::UpdateViewFeel(float dt){
 const auto area=ultimo::AreaAt(camera.Position.x,camera.Position.y,camera.Position.z);
 const Grade grade=GradeFor(IsMenuBackdrop()||investigation.screen==Screen::Intro?ultimo::AcousticArea::Coast:area);
 const bool exploring=investigation.screen==Screen::Explore&&!experience.menuCameraHeld;
 const float focusTarget=exploring&&target>=0&&target<int(interactionPositions.size())?1.f:0.f;
 if(focusTarget>0)focusPosition=interactionPositions[target];
 if(benchmarkMode||dt<=0){
  // Deterministic review frames: no easing between states.
  gradeShadow=grade.shadow;gradeHighlight=grade.highlight;gradeSaturation=grade.saturation;
  focusFade=focusTarget;
  if(benchmarkMode){renderLift=renderZoom=0;bobAmount=0;smoothEyeY=camera.Position.y;return;}
 }else{
  const float ease=1.f-std::exp(-dt*1.6f);
  gradeShadow+=(grade.shadow-gradeShadow)*ease;gradeHighlight+=(grade.highlight-gradeHighlight)*ease;
  gradeSaturation+=(grade.saturation-gradeSaturation)*ease;
  focusFade+=(focusTarget-focusFade)*(1.f-std::exp(-dt*(focusTarget>focusFade?14.f:7.f)));
 }
 if(!exploring){renderLift=renderZoom=0;bobAmount=0;smoothEyeY=camera.Position.y;return;}
 // Each 20 cm riser becomes a short glide instead of a visible jump.
 const float gap=camera.Position.y-smoothEyeY;
 if(std::abs(gap)>.75f)smoothEyeY=camera.Position.y;
 else smoothEyeY+=gap*(1.f-std::exp(-dt*13.f));
 const float planar=std::hypot(velocity.x,velocity.z);
 const float swayTarget=(!settings.reduceMotion&&walking)?(std::min)(1.f,planar/2.6f):0.f;
 bobAmount+=(swayTarget-bobAmount)*(1.f-std::exp(-dt*8.f));
 // 0.47 s per footfall, the cadence the audio mixer already uses.
 bobPhase=std::fmod(bobPhase+dt*13.37f,6.2831853f);
 renderLift=(smoothEyeY-camera.Position.y)+std::sin(bobPhase)*.014f*bobAmount;
 const float zoomTarget=(!settings.reduceMotion&&sprinting&&walking)?2.5f:0.f;
 renderZoom+=(zoomTarget-renderZoom)*(1.f-std::exp(-dt*6.f));
}

void UltimoSinal::DrawTechniquePanel(){
 const auto display=ImGui::GetIO().DisplaySize;
 if(display.x<1||display.y<1)return;
 ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.035f,.055f,.065f,.93f));
 ImGui::PushStyleColor(ImGuiCol_Text,ImVec4(.89f,.87f,.80f,1));
 ImGui::PushStyleColor(ImGuiCol_CheckMark,ImVec4(.84f,.68f,.39f,1));
 ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(.11f,.15f,.17f,1));
 ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(16,14));
 ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,3.f);
 ImGui::SetNextWindowPos(ImVec2(display.x-20,20),ImGuiCond_Always,ImVec2(1,0));
 ImGui::SetNextWindowSize(ImVec2((std::min)(400.f,(std::max)(220.f,display.x-40)),0),ImGuiCond_Always);
 ImGui::Begin("##technique-panel",nullptr,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoSavedSettings);
 ImGui::TextColored(ImVec4(.84f,.68f,.39f,1),"DEMONSTRAÇÃO DAS TÉCNICAS  /  F3");
 ImGui::TextWrapped("Desligue um item para comparar a mesma vista com e sem a técnica. Nada aqui é salvo nem altera a investigação.");
 ImGui::Separator();
 ImGui::Checkbox("Multitextura no terreno (grama + pedra)",&multiEnabled);
 ImGui::Checkbox("Normal mapping na pedra",&normalEnabled);
 ImGui::Checkbox("Luzes Point (luminárias e lente)",&pointEnabled);
 ImGui::Checkbox("Luz Spot (lanterna do jogador)",&spotEnabled);
 ImGui::Checkbox("Environment mapping: reflexão e refração",&environmentEnabled);
 ImGui::Checkbox("Fog por distância",&fogEnabled);
 ImGui::Separator();
 ImGui::TextColored(ImVec4(.56f,.62f,.63f,1),"ACABAMENTO EM TELA (qualidade Média ou Alta)");
 ImGui::Checkbox("Pós-processamento HDR",&postEnabled);
 ImGui::BeginDisabled(!postEnabled||post.failed||settings.quality==0);
 ImGui::Checkbox("Oclusão de ambiente (SSAO)",&aoEnabled);
 ImGui::Checkbox("Bloom nas fontes de luz",&bloomEnabled);
 ImGui::EndDisabled();
 if(post.failed)ImGui::TextWrapped("Este driver não aceitou o alvo HDR; a cena usa o desenho direto.");
 else if(settings.quality==0)ImGui::TextWrapped("Na qualidade Baixa a cena usa o desenho direto, sem estes passes.");
 ImGui::Separator();
 ImGui::TextWrapped("Skybox, blending de água e vidro e o bake do piso (CompleteMap, UV1) permanecem sempre ativos.");
 if(ImGui::Button("Religar tudo",ImVec2(-1,30))){
  multiEnabled=normalEnabled=pointEnabled=spotEnabled=environmentEnabled=fogEnabled=true;
  postEnabled=aoEnabled=bloomEnabled=true;
 }
 ImGui::TextDisabled("F3 fecha o painel e devolve o mouse à câmera.");
 ImGui::End();
 ImGui::PopStyleVar(2);ImGui::PopStyleColor(4);
}
