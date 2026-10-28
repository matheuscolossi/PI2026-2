#include "UltimoSinal.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace {
constexpr int PreviewWidth=640,PreviewHeight=320;
// Rendering a thumbnail must leave both the world and ImGui's subsequent draw
// untouched, including split read/draw framebuffers and a caller's scissor.
struct PreviewState {
 GLint drawFbo,readFbo,viewport[4],program,vao,active,texture,renderbuffer,depthFunction;
 GLboolean depth,blend,cull,scissor,depthMask,colorMask[4];GLfloat clear[4];
 PreviewState(){
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,&drawFbo);glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,&readFbo);
  glGetIntegerv(GL_VIEWPORT,viewport);glGetIntegerv(GL_CURRENT_PROGRAM,&program);glGetIntegerv(GL_VERTEX_ARRAY_BINDING,&vao);
  glGetIntegerv(GL_ACTIVE_TEXTURE,&active);glActiveTexture(GL_TEXTURE0);glGetIntegerv(GL_TEXTURE_BINDING_2D,&texture);
  glGetIntegerv(GL_RENDERBUFFER_BINDING,&renderbuffer);glGetIntegerv(GL_DEPTH_FUNC,&depthFunction);
  depth=glIsEnabled(GL_DEPTH_TEST);blend=glIsEnabled(GL_BLEND);cull=glIsEnabled(GL_CULL_FACE);scissor=glIsEnabled(GL_SCISSOR_TEST);
  glGetBooleanv(GL_DEPTH_WRITEMASK,&depthMask);glGetBooleanv(GL_COLOR_WRITEMASK,colorMask);glGetFloatv(GL_COLOR_CLEAR_VALUE,clear);
 }
 ~PreviewState(){
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,drawFbo);glBindFramebuffer(GL_READ_FRAMEBUFFER,readFbo);
  glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);glUseProgram(program);glBindVertexArray(vao);
  glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D,texture);glActiveTexture(active);glBindRenderbuffer(GL_RENDERBUFFER,renderbuffer);
  auto restore=[](GLenum cap,GLboolean value){if(value)glEnable(cap);else glDisable(cap);};
  restore(GL_DEPTH_TEST,depth);restore(GL_BLEND,blend);restore(GL_CULL_FACE,cull);restore(GL_SCISSOR_TEST,scissor);
  glDepthFunc(depthFunction);glDepthMask(depthMask);glColorMask(colorMask[0],colorMask[1],colorMask[2],colorMask[3]);glClearColor(clear[0],clear[1],clear[2],clear[3]);
 }
};
}

void UltimoSinal::StartLensPlayback(){
 if(!investigation.lensSolved||lensPlaybackTime>=0)return;
 lensPlaybackTime=0;audio.Cue(5);
}

bool UltimoSinal::RenderEvidencePreview(int id){
 const char* name=id==0?"diario":id==1?"alicate":id==3?"chave":id==4?"foto":nullptr;
 if(!name)return false;LoadWorldAssets();auto model=worldAssets.find(name);if(model==worldAssets.end())return false;
 PreviewState restore;
 if(!evidenceFbo){
  glGenFramebuffers(1,&evidenceFbo);glGenTextures(1,&evidenceTexture);glGenRenderbuffers(1,&evidenceDepth);
  glBindTexture(GL_TEXTURE_2D,evidenceTexture);glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,PreviewWidth,PreviewHeight,0,GL_RGBA,GL_UNSIGNED_BYTE,nullptr);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glBindRenderbuffer(GL_RENDERBUFFER,evidenceDepth);glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,PreviewWidth,PreviewHeight);
  glBindFramebuffer(GL_FRAMEBUFFER,evidenceFbo);glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,evidenceTexture,0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,evidenceDepth);
 }
 glBindFramebuffer(GL_FRAMEBUFFER,evidenceFbo);if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)return false;
 glViewport(0,0,PreviewWidth,PreviewHeight);glDisable(GL_SCISSOR_TEST);glDisable(GL_BLEND);glDisable(GL_CULL_FACE);
 glEnable(GL_DEPTH_TEST);glDepthFunc(GL_LESS);glDepthMask(GL_TRUE);glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
 glClearColor(.028f,.047f,.050f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 const auto& report=model->second->GetReport();const auto size=report.boundsMax-report.boundsMin;
 const auto longest=(std::max)({size.x,size.y,size.z,.001f});
 auto transform=glm::rotate(glm::mat4(1),glm::radians(evidenceYaw),glm::vec3(0,1,0));
 transform=glm::rotate(transform,glm::radians(evidencePitch),glm::vec3(1,0,0));
 transform=glm::scale(transform,glm::vec3(1.65f/longest*evidenceZoom));
 transform=glm::translate(transform,-(report.boundsMin+report.boundsMax)*.5f);
 shader.Use("Inspection");shader.SetMat4("model",transform);
 shader.SetMat4("view",glm::lookAt(glm::vec3(0,1.65f,2.65f),glm::vec3(0),glm::vec3(0,1,0)));
 shader.SetMat4("projection",glm::perspective(glm::radians(38.f),float(PreviewWidth)/PreviewHeight,.05f,10.f));
 AssetDrawOptions options;model->second->Draw(shader.GetProgram("Inspection"),options);
 return true;
}

void UltimoSinal::DrawEvidencePreview(int id){
 if(previewEvidence!=id){previewEvidence=id;evidenceYaw=15;evidencePitch=0;evidenceZoom=1;}
 if(!RenderEvidencePreview(id)){ImGui::TextWrapped("A visualização está indisponível. O registro acima continua completo.");return;}
 const float width=(std::min)(420.f,ImGui::GetContentRegionAvail().x);
 const float height=width*.5f;
 ImGui::Image((ImTextureID)(intptr_t)evidenceTexture,ImVec2(width,height),ImVec2(0,1),ImVec2(1,0));
 if(ImGui::IsItemHovered()&&ImGui::IsMouseDragging(ImGuiMouseButton_Left)){
  const auto delta=ImGui::GetIO().MouseDelta;evidenceYaw=std::fmod(evidenceYaw+delta.x*.6f,360.f);evidencePitch=(std::clamp)(evidencePitch+delta.y*.5f,-85.f,85.f);
 }
 ImGui::TextDisabled("Arraste sobre o objeto para girar. As opções abaixo também aceitam teclado.");
 ImGui::SliderFloat("Rotação",&evidenceYaw,-180.f,180.f,"%.0f°");
 ImGui::SliderFloat("Inclinação",&evidencePitch,-85.f,85.f,"%.0f°");
 ImGui::SliderFloat("Aproximação",&evidenceZoom,.7f,1.5f,"%.1fx");
 if(ImGui::Button("Restaurar vista")){evidenceYaw=15;evidencePitch=0;evidenceZoom=1;}
}
