#pragma once
#include "Engine/CSceneBase.h"
#include "Engine/CCamera.h"
#include "Engine/CPrimitives.h"
#include "Engine/CShader.h"
#include "Investigation.h"
#include "Geometry.h"
#include <chrono>

class UltimoSinal final : public CSceneBase {
public:
    UltimoSinal();
    ~UltimoSinal() override;
    void ProcessSceneInput(GLFWwindow* window,float deltaTime) override;
    void DrawScene(float deltaTime) override;
private:
    struct Block { ultimo::Bounds bounds;glm::vec3 color;bool solid; };
    ultimo::Investigation investigation;
    CCamera camera{glm::vec3(0,1.7f,17)};
    CShader shader;
    CPrimitives cube,cylinder,cone;
    std::vector<Block> blocks;
    std::array<glm::vec3,ultimo::ClueCount> cluePositions{{{3,1.13f,2},{-3,1.13f,3},{0,.12f,8},{-3,1.13f,1},{3,1.13f,-3}}};
    ultimo::Bounds door{{0,1.35f,-.5f},{2,2.7f,.25f}};
    int target=-1;
    bool captured=false,skipMouse=true;
    double duration=0;
    std::chrono::steady_clock::time_point start;
    void Start();
    void SyncCursor();
    bool Blocked(glm::vec3 p) const;
    bool Visible(glm::vec3 destination) const;
    void FindTarget();
    void Render(CPrimitives& primitive,glm::vec3 center,glm::vec3 size,glm::vec3 color);
    void DrawWorld();
    void DrawInterface();
};
