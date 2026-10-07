#include "UltimoSinal.h"
#include <algorithm>
#include <cmath>

using ultimo::Screen;
using ultimo::ClueId;
static glm::vec3 V(ultimo::Vec3 v){return {v.x,v.y,v.z};}

UltimoSinal::UltimoSinal() {
    camera.Zoom=58.f;
    cube.generateCube(1);cube.setupBuffers();
    cylinder.generateCylinder(1,1);cylinder.setupBuffers();
    cone.generateCone(1,1);cone.setupBuffers();
    shader.LoadShader("Prototype","Scenes/UltimoSinal/Prototype.vert","Scenes/UltimoSinal/Prototype.frag");
    shader.LoadShader("SkyBox","Scenes/Common/SkyBox.vert","Scenes/UltimoSinal/Sky.frag");
    skybox=std::make_unique<CSkybox>(std::vector<std::string>{"Scenes/Skybox/right.bmp","Scenes/Skybox/left.bmp","Scenes/Skybox/top.bmp","Scenes/Skybox/bottom.bmp","Scenes/Skybox/front.bmp","Scenes/Skybox/back.bmp"});
    textures.LoadTextureMipMap(0,"Scenes/Textures/grass.bmp");
    textures.LoadTextureMipMap(1,"Scenes/Textures/rock.jpg");
    textures.LoadTextureMipMap(2,"Scenes/Textures/rockNormal.jpg");
    blocks={
        {{{0,-.8f,0},{110,.5f,110}},{.035f,.13f,.18f},false},
        {{{0,-.3f,0},{26,.6f,42}},{.23f,.30f,.24f},false},
        {{{0,.03f,10.5f},{2.4f,.06f,13}},{.53f,.48f,.36f},false},
        {{{0,.03f,-.5f},{10,.06f,11}},{.30f,.28f,.24f},false},
        {{{-5,1.6f,-.5f},{.3f,3.2f,11}},{.58f,.61f,.58f},true},
        {{{5,1.6f,-.5f},{.3f,3.2f,11}},{.50f,.53f,.50f},true},
        {{{0,1.6f,-6},{10,3.2f,.3f}},{.48f,.51f,.49f},true},
        {{{-3,1.6f,5},{4,3.2f,.3f}},{.68f,.69f,.63f},true},
        {{{3,1.6f,5},{4,3.2f,.3f}},{.68f,.69f,.63f},true},
        {{{0,2.95f,5},{2,.5f,.3f}},{.68f,.69f,.63f},false},
        {{{-3,1.6f,-.5f},{4,3.2f,.3f}},{.48f,.52f,.48f},true},
        {{{3,1.6f,-.5f},{4,3.2f,.3f}},{.48f,.52f,.48f},true},
        {{{0,3.25f,-.5f},{10,.15f,11}},{.33f,.35f,.33f},false},
        {{{3,.5f,2},{1.6f,1,.9f}},{.31f,.23f,.15f},true},
        {{{-3,.5f,3},{1.6f,1,.9f}},{.31f,.23f,.15f},true},
        {{{-3,.5f,1},{1.6f,1,.9f}},{.31f,.23f,.15f},true},
        {{{3,.5f,-3},{1.6f,1,.9f}},{.31f,.23f,.15f},true}
    };
    // A cena do professor captura o cursor antes de construir qualquer cena.
    InputManager::GetInstance().SetCursorMode(GLFW_CURSOR_NORMAL);
}
UltimoSinal::~UltimoSinal(){cube.cleanup();cylinder.cleanup();cone.cleanup();}

void UltimoSinal::Start() {
    investigation.Start();camera=CCamera({0,1.7f,17});camera.Zoom=58;
    answer.fill(0);target=-1;duration=0;start=std::chrono::steady_clock::now();
    skipMouse=true;SyncCursor();
}
void UltimoSinal::SyncCursor() {
    bool desired=investigation.screen==Screen::Explore && !benchmarkMode;
    if(captured!=desired){captured=desired;skipMouse=true;
        InputManager::GetInstance().SetCursorMode(captured?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);}
}
bool UltimoSinal::Blocked(glm::vec3 p) const {
    if(std::abs(p.x)>12.5f || std::abs(p.z)>20.5f)return true;
    for(const auto& b:blocks)if(b.solid && ultimo::BlocksWalker(p.x,p.z,b.bounds))return true;
    return !investigation.DoorOpen() && ultimo::BlocksWalker(p.x,p.z,door);
}
bool UltimoSinal::Visible(glm::vec3 p) const {
    ultimo::Vec3 a{camera.Position.x,camera.Position.y,camera.Position.z},b{p.x,p.y,p.z};
    for(const auto& block:blocks)if(block.solid && ultimo::SegmentHits(a,b,block.bounds))return false;
    return investigation.DoorOpen() || !ultimo::SegmentHits(a,b,door);
}
void UltimoSinal::FindTarget() {
    target=-1;float best=.965f;
    for(std::size_t i=0;i<cluePositions.size();++i) {
        auto offset=cluePositions[i]-camera.Position;float distance=glm::length(offset);
        if(distance<2.8f && distance>.05f) {
            float alignment=glm::dot(glm::normalize(offset),camera.Front);
            if(alignment>best && Visible(cluePositions[i])){target=int(i);best=alignment;}
        }
    }
}
void UltimoSinal::ProcessSceneInput(GLFWwindow* window,float deltaTime) {
    auto& input=InputManager::GetInstance();
    if(investigation.screen!=Screen::Title && investigation.screen!=Screen::Complete)
        duration=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
    if(!glfwGetWindowAttrib(window,GLFW_FOCUSED)) {
        if(investigation.screen==Screen::Explore)investigation.TogglePause();
        SyncCursor();skipMouse=true;return;
    }
    if(ImGui::GetIO().WantTextInput)return;
    const auto before=investigation.screen;
    if(before==Screen::Title && input.IsKeyJustPressed(GLFW_KEY_ENTER)){Start();return;}
    if(input.IsKeyJustPressed(GLFW_KEY_TAB))investigation.TogglePause();
    if(input.IsKeyJustPressed(GLFW_KEY_J))investigation.ToggleJournal();
    if(before==Screen::Reading && input.IsKeyJustPressed(GLFW_KEY_E))investigation.CloseReading();
    if((before==Screen::Pause || before==Screen::Complete) && input.IsKeyJustPressed(GLFW_KEY_R)){Start();return;}
    if(investigation.screen!=before){SyncCursor();return;}
    if(investigation.screen!=Screen::Explore)return;
    auto mouse=input.GetMouseDelta();
    camera.MouseSensitivity=settings.Sensitivity();
    if(skipMouse)skipMouse=false;else camera.ProcessMouseMovement(mouse.x,settings.invertY?-mouse.y:mouse.y);
    glm::vec3 forward=camera.Front;forward.y=0;forward=glm::normalize(forward);
    glm::vec3 right=camera.Right;right.y=0;right=glm::normalize(right);
    glm::vec3 movement(0);
    if(input.IsKeyPressed(GLFW_KEY_W))movement+=forward;
    if(input.IsKeyPressed(GLFW_KEY_S))movement-=forward;
    if(input.IsKeyPressed(GLFW_KEY_D))movement+=right;
    if(input.IsKeyPressed(GLFW_KEY_A))movement-=right;
    Walk(movement,deltaTime);
    FindTarget();
    if(input.IsKeyJustPressed(GLFW_KEY_E) && target>=0)investigation.Inspect(static_cast<ClueId>(target));
    if(input.IsKeyJustPressed(GLFW_KEY_ENTER))investigation.BeginConclusion();
    SyncCursor();
}
void UltimoSinal::DrawScene(float) {
    glClearColor(.07f,.12f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    DrawWorld();DrawInterface();
}

void UltimoSinal::Walk(glm::vec3 movement,float deltaTime){
    if(glm::length(movement)>.001f) {
        float dt=std::clamp(deltaTime,0.f,.1f);int steps=(std::max)(1,int(std::ceil(dt/.02f)));
        movement=glm::normalize(movement)*3.f*dt/float(steps);
        for(int n=0;n<steps;++n){auto p=camera.Position;p.x+=movement.x;if(!Blocked(p))camera.Position=p;
            p=camera.Position;p.z+=movement.z;if(!Blocked(p))camera.Position=p;}
    }
}
