#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CPrimitives.h"
#include "Engine/CShader.h"
#include "Investigation.h"
#include "Geometry.h"
#include "Settings.h"
#include "Engine/CSkybox.h"
#include "Engine/CTextures.h"
#include <memory>
#include <chrono>

class UltimoSinal final : public CSceneBase {
public:
    UltimoSinal();
    ~UltimoSinal() override;
    void ProcessSceneInput(GLFWwindow* window,float deltaTime) override;
    void DrawScene(float deltaTime) override;
    bool VerifyRuntime();
    void BenchmarkPose(float time);
private:
    struct Block { ultimo::Bounds bounds;glm::vec3 color;bool solid; };
    ultimo::Investigation investigation;
    ultimo::Settings settings;
    std::array<char,2048> answer{};
    void DrawSettings();
    CCamera camera{glm::vec3(0,1.7f,17)};
    CShader shader;
    CTextures textures;
    std::unique_ptr<CSkybox> skybox;
    bool normalEnabled=true,fogEnabled=true,pointEnabled=true,spotEnabled=true,environmentEnabled=true,multiEnabled=true;
    CPrimitives cube,cylinder,cone;
    std::vector<Block> blocks;
    std::array<glm::vec3,ultimo::ClueCount> cluePositions{{{3,1.13f,2},{-3,1.13f,3},{0,.12f,8},{.7f,.15f,6.4f},{3,1.13f,-3}}};
    ultimo::Bounds door{{0,1.35f,-.5f},{2,2.7f,.25f}};
    int target=-1;
    bool captured=false,skipMouse=true,benchmarkMode=false;
    double duration=0;
    std::chrono::steady_clock::time_point start;
    void Start();
    void Walk(glm::vec3 movement,float deltaTime);
    void SyncCursor();
    bool Blocked(glm::vec3 p) const;
    bool Visible(glm::vec3 destination) const;
    void FindTarget();
    void Render(CPrimitives& primitive,glm::vec3 center,glm::vec3 size,glm::vec3 color,int material=0,float alpha=1);
    void DrawWorld();
    void DrawInterface();
};
