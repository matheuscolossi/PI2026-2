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
    target=-1;duration=0;start=std::chrono::steady_clock::now();
    skipMouse=true;SyncCursor();
}
void UltimoSinal::SyncCursor() {
    bool desired=investigation.screen==Screen::Explore;
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
    const auto before=investigation.screen;
    if(before==Screen::Title && input.IsKeyJustPressed(GLFW_KEY_ENTER)){Start();return;}
    if(input.IsKeyJustPressed(GLFW_KEY_TAB))investigation.TogglePause();
    if(input.IsKeyJustPressed(GLFW_KEY_J))investigation.ToggleJournal();
    if(before==Screen::Reading && input.IsKeyJustPressed(GLFW_KEY_E))investigation.CloseReading();
    if((before==Screen::Pause || before==Screen::Complete) && input.IsKeyJustPressed(GLFW_KEY_R)){Start();return;}
    if(investigation.screen!=before){SyncCursor();return;}
    if(investigation.screen!=Screen::Explore)return;
    auto mouse=input.GetMouseDelta();
    if(skipMouse)skipMouse=false;else camera.ProcessMouseMovement(mouse.x,mouse.y);
    glm::vec3 forward=camera.Front;forward.y=0;forward=glm::normalize(forward);
    glm::vec3 right=camera.Right;right.y=0;right=glm::normalize(right);
    glm::vec3 movement(0);
    if(input.IsKeyPressed(GLFW_KEY_W))movement+=forward;
    if(input.IsKeyPressed(GLFW_KEY_S))movement-=forward;
    if(input.IsKeyPressed(GLFW_KEY_D))movement+=right;
    if(input.IsKeyPressed(GLFW_KEY_A))movement-=right;
    if(glm::length(movement)>.001f) {
        float dt=std::clamp(deltaTime,0.f,.1f);int steps=std::max(1,int(std::ceil(dt/.02f)));
        movement=glm::normalize(movement)*3.f*dt/float(steps);
        for(int n=0;n<steps;++n){auto p=camera.Position;p.x+=movement.x;if(!Blocked(p))camera.Position=p;
            p=camera.Position;p.z+=movement.z;if(!Blocked(p))camera.Position=p;}
    }
    FindTarget();
    if(input.IsKeyJustPressed(GLFW_KEY_E) && target>=0)investigation.Inspect(static_cast<ClueId>(target));
    if(input.IsKeyJustPressed(GLFW_KEY_ENTER))investigation.FinishPrototype();
    SyncCursor();
}
void UltimoSinal::Render(CPrimitives& primitive,glm::vec3 center,glm::vec3 size,glm::vec3 color) {
    auto model=glm::scale(glm::translate(glm::mat4(1),center),size);
    shader.SetMat4("model",model);shader.SetVec4("Color",glm::vec4(color,1));primitive.render();
}
void UltimoSinal::DrawWorld() {
    int width,height;glfwGetFramebufferSize(glfwGetCurrentContext(),&width,&height);
    if(width<=0 || height<=0)return;
    shader.Use("Prototype");shader.SetBool("isTextured",false);
    shader.SetMat4("view",camera.GetViewMatrix());
    shader.SetMat4("projection",glm::perspective(glm::radians(camera.Zoom),float(width)/height,.1f,160.f));
    for(const auto& b:blocks)Render(cube,V(b.bounds.center),V(b.bounds.size),b.color);
    Render(cylinder,{0,5.4f,-3},{2.3f,4.2f,2.3f},{.70f,.70f,.63f});
    Render(cylinder,{0,7.6f,-3},{2.6f,.25f,2.6f},{.25f,.30f,.29f});
    Render(cylinder,{0,8.3f,-3},{1.6f,1.1f,1.6f},{.94f,.73f,.35f});
    Render(cone,{0,9.15f,-3},{2.4f,.7f,2.4f},{.30f,.20f,.17f});
    if(!investigation.DoorOpen())Render(cube,V(door.center),V(door.size),{.36f,.25f,.16f});
    for(std::size_t i=0;i<cluePositions.size();++i) {
        glm::vec3 color=investigation.Found(static_cast<ClueId>(i))?glm::vec3(.30f,.65f,.55f):glm::vec3(.86f,.60f,.28f);
        if(int(i)==target && investigation.screen==Screen::Explore)color={1,.85f,.40f};
        Render(cube,cluePositions[i],i==2?glm::vec3(.6f,.12f,.8f):glm::vec3(.55f,.16f,.35f),color);
    }
}
void UltimoSinal::DrawInterface() {
    const auto display=ImGui::GetIO().DisplaySize;
    const auto fixed=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse;
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.04f,.08f,.11f,.96f));
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.15f,.35f,.36f,1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(.22f,.46f,.44f,1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(20,16));
    if(investigation.screen==Screen::Explore) {
        ImGui::SetNextWindowPos({18,18});ImGui::SetNextWindowBgAlpha(.88f);
        ImGui::Begin("O ULTIMO SINAL",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Pistas: %zu / 5",investigation.Count());
        ImGui::TextUnformatted(investigation.DoorOpen()?"Explore a sala da lente.":"Entre no farol e encontre a chave.");
        ImGui::Separator();ImGui::TextUnformatted("WASD: andar | Mouse: olhar | E: examinar");
        ImGui::TextUnformatted("J: caderno | Tab: pausa | Esc: sair");
        if(investigation.AllFound())ImGui::TextUnformatted("Enter: encerrar teste das cinco pistas.");
        ImGui::End();
        ImGui::GetForegroundDrawList()->AddCircleFilled({display.x*.5f,display.y*.5f},3,IM_COL32(255,233,184,255));
        if(target>=0) {
            ImGui::SetNextWindowPos({display.x*.5f,display.y*.82f},ImGuiCond_Always,{.5f,0});
            ImGui::Begin("Examinar",nullptr,fixed|ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("E - %s",ultimo::ClueTexts[target].name);ImGui::End();
        }
    } else {
        ImGui::SetNextWindowPos({display.x*.5f,display.y*.5f},ImGuiCond_Always,{.5f,.5f});
        ImGui::SetNextWindowSize({std::min(560.f,display.x-32),0});
        ImGui::Begin("O ULTIMO SINAL",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize);
        if(investigation.screen==Screen::Title) {
            ImGui::TextUnformatted("Uma ilha. Um farol. Cinco pistas.");ImGui::Separator();
            ImGui::TextWrapped("Explore o farol e examine os objetos. A chave abre a sala da lente. Consulte suas descobertas no caderno.");
            ImGui::Spacing();
            if(ImGui::Button("Iniciar exploracao  [Enter]",{-1,38}))Start();
            ImGui::TextDisabled("Versao inicial - cenarios e narrativa provisorios.");
            ImGui::TextUnformatted("WASD / Mouse / E / J / Tab | Esc: sair");
        } else if(investigation.screen==Screen::Reading) {
            const auto& clue=ultimo::ClueTexts[static_cast<std::size_t>(investigation.reading)];
            ImGui::TextUnformatted(clue.name);ImGui::Separator();ImGui::TextWrapped("%s",clue.description);
            ImGui::Spacing();if(ImGui::Button("Voltar a explorar  [E]",{-1,32}))investigation.CloseReading();
        } else if(investigation.screen==Screen::Journal) {
            ImGui::Text("CADERNO | %zu de 5 pistas",investigation.Count());ImGui::Separator();
            for(std::size_t i=0;i<ultimo::ClueCount;++i) {
                auto id=static_cast<ClueId>(i);ImGui::PushID(int(i));
                if(investigation.Found(id)){if(ImGui::Button(ultimo::ClueTexts[i].name,{-1,30}))investigation.ReadAgain(id);}
                else ImGui::TextDisabled("[ ] %s",ultimo::ClueTexts[i].name);
                ImGui::PopID();
            }
            ImGui::Spacing();if(ImGui::Button("Fechar caderno  [J]",{-1,32}))investigation.ToggleJournal();
        } else if(investigation.screen==Screen::Pause) {
            ImGui::TextUnformatted("PAUSADO");ImGui::Separator();
            if(ImGui::Button("Continuar  [Tab]",{-1,32}))investigation.TogglePause();
            if(ImGui::Button("Reiniciar exploracao  [R]",{-1,32}))Start();
            ImGui::TextWrapped("Aulas preservadas: teclas 0, 1 e 5. Tecla 2: reinicia a cena do jogo.");
        } else {
            ImGui::TextUnformatted("PERCURSO DO PROTOTIPO CONCLUIDO");ImGui::Separator();
            ImGui::TextWrapped("As cinco pistas foram localizadas. A escolha de conclusao e o feedback do misterio dependem da revisao do roteiro.");
            ImGui::Text("Tempo de sessao: %.0f s (inclui leitura e pausas)",duration);
            if(ImGui::Button("Reiniciar  [R]",{-1,32}))Start();
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();ImGui::PopStyleColor(3);SyncCursor();
}
void UltimoSinal::DrawScene(float) {
    glClearColor(.07f,.12f,.16f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    DrawWorld();DrawInterface();
}
