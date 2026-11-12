#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CPrimitives.h"
#include "Engine/CShader.h"
#include "Engine/CSkybox.h"
#include "Engine/CTextures.h"
#include "Engine/CAsset.h"
#include "Investigation.h"
#include "Geometry.h"
#include "Spatial.h"
#include "Settings.h"
#include "Audio.h"
#include "Experience.h"
#include "Atmosphere.h"
#include "Drama.h"
#include <memory>
#include <chrono>
#include <unordered_map>
class UltimoSinal final:public CSceneBase{
public:
 UltimoSinal();~UltimoSinal()override;
 void ProcessSceneInput(GLFWwindow*,float)override;void DrawScene(float)override;
 bool VerifyRuntime();bool RenderReview();void PreviewTables();void BenchmarkPose(float);
private:
 // Optional HDR targets and passes (PostFx.cpp). A failure only clears `active`.
 struct PostFx{
  GLuint program=0,vao=0,sceneFbo=0,sceneColor=0,sceneDepth=0,resolveFbo=0,resolveColor=0,resolveDepth=0,occlusionFbo=0,occlusionTexture=0;
  std::array<GLuint,5> bloomFbo{},bloomTexture{};std::array<int,5> bloomWidth{},bloomHeight{};
  int width=0,height=0,samples=0;GLint previousDraw=0,previousRead=0;bool failed=false,active=false;
 };
 PostFx post;ultimo::Drama drama;
 glm::mat4 lastProjection{1.f};
 glm::vec3 velocity{0.f},focusPosition{0.f},gradeShadow{1.f},gradeHighlight{1.f};
 float gradeSaturation=1,focusFade=0,smoothEyeY=1.7f,renderLift=0,renderZoom=0,bobPhase=0,bobAmount=0;
 bool postEnabled=true,aoEnabled=true,bloomEnabled=true,techniquePanel=false,mouseHeld=false,sprinting=false;
 ultimo::AtmosphereMoment beatMoment=ultimo::AtmosphereMoment::None;
 void RenderWorld();bool BeginPost();void EndPost();bool PreparePost(int,int);void ReleasePost(bool keepProgram);
 void UpdateViewFeel(float);void DrawTechniquePanel();
 struct Block{ultimo::Bounds bounds;glm::vec3 color;bool solid;};
 struct DeskInstance{glm::mat4 model{1.f};ultimo::Bounds bounds;};
 ultimo::Experience experience;
 ultimo::Atmosphere atmosphere;
 ultimo::Investigation investigation;ultimo::Settings settings;ultimo::Audio audio;
 CCamera camera{glm::vec3(0,1.7f,17)};CShader shader;CTextures textures;std::unique_ptr<CSkybox> skybox;
 CPrimitives cube,cylinder,cone,waterPlane;std::vector<Block> blocks;
 std::unique_ptr<CAsset> mesa;std::vector<DeskInstance> desks;
 std::unordered_map<std::string,std::unique_ptr<CAsset>> worldAssets;bool worldAssetsScanned=false,worldCoverageLogged=false,floorUsesCompleteMap=false;
 std::array<glm::vec3,5> cluePositions{{{-2.9f,1.13f,1.8f},{-3,1.13f,.25f},{-2,.035f,9},{-3.7f,1.25f,3.8f},{3,1.13f,-3}}};
 // 0..9: evidence entries, 10: panel, 11: radio, 12: isolated lens test console. Positions shared by interaction and art.
 std::array<glm::vec3,13> interactionPositions{};
 ultimo::Bounds door{{0,1.35f,-.5f},{2,2.7f,.25f}};
 bool normalEnabled=true,fogEnabled=true,pointEnabled=true,spotEnabled=true,environmentEnabled=true,multiEnabled=true;
  bool captured=false,skipMouse=true,benchmarkMode=false,hasSave=false,walking=false,sessionActive=false;
 float autosave=0,worldTime=0,doorAmount=0,toastTime=0;double duration=0;
 int target=-1,menuReturn=int(ultimo::Screen::Title),remapping=-1;
 int inspectedEvidence=-1,previewEvidence=-1;
 GLuint evidenceFbo=0,evidenceTexture=0,evidenceDepth=0;
 float evidenceYaw=0,evidencePitch=0,evidenceZoom=1,lensPlaybackTime=-1;
 bool RenderEvidencePreview(int);void DrawEvidencePreview(int);void StartLensPlayback();
 std::array<char,2048> answer{};std::string saveNotice;
 void Start();void SyncCursor();void Walk(glm::vec3,float,float speed=2.6f);bool Blocked(glm::vec3)const;bool Visible(glm::vec3)const;void FindTarget();
 void Interact();void Save();bool Load();void DeleteSave();void LoadSettings();void SaveSettings();
 void LoadDesk();void LoadWorldAssets();void BuildWorld();void Render(CPrimitives&,glm::vec3,glm::vec3,glm::vec3,int material=0,float alpha=1);
 void DrawWorld();void DrawInterface();void DrawSettings();
 bool IsMenuBackdrop()const;void RestorePlayerCamera();void ResetExperience(bool);void FinishIntro();void UpdateExperience(float);void DrawExperience(bool allowBenchmark=false);
};
