#include "UltimoSinal.h"
#include <array>
#include <cfloat>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <functional>
#include <cmath>
#include <vector>

namespace {
// Export only this engine's OpenGL back buffer. This does not capture a desktop,
// another application's window, or any operating-system surface.
bool WriteBackBufferBmp(const std::filesystem::path& path,int width,int height){
    if(width<=0||height<=0||width>8192||height>8192)return false;
    const std::uint32_t stride=(std::uint32_t(width)*3u+3u)&~3u;
    std::vector<unsigned char> rgb(std::size_t(width)*height*3u);
    GLint oldAlignment=4,oldReadBuffer=GL_BACK;
    glGetIntegerv(GL_PACK_ALIGNMENT,&oldAlignment);glGetIntegerv(GL_READ_BUFFER,&oldReadBuffer);
    glPixelStorei(GL_PACK_ALIGNMENT,1);glReadBuffer(GL_BACK);glFinish();
    glReadPixels(0,0,width,height,GL_RGB,GL_UNSIGNED_BYTE,rgb.data());
    const GLenum error=glGetError();
    glPixelStorei(GL_PACK_ALIGNMENT,oldAlignment);glReadBuffer(GLenum(oldReadBuffer));
    if(error!=GL_NO_ERROR){std::cerr<<"Render review readback GL error="<<error<<std::endl;return false;}
    std::ofstream out(path,std::ios::binary|std::ios::trunc);if(!out)return false;
    const auto u16=[&](std::uint16_t value){out.put(char(value&255));out.put(char((value>>8)&255));};
    const auto u32=[&](std::uint32_t value){for(int i=0;i<4;++i)out.put(char((value>>(i*8))&255));};
    out.put('B');out.put('M');u32(54u+stride*std::uint32_t(height));u16(0);u16(0);u32(54);
    u32(40);u32(std::uint32_t(width));u32(std::uint32_t(height));u16(1);u16(24);u32(0);
    u32(stride*std::uint32_t(height));u32(2835);u32(2835);u32(0);u32(0);
    std::vector<unsigned char> row(stride,0);
    // Both OpenGL readback and positive-height BMP store the bottom row first.
    for(int y=0;y<height;++y){
        for(int x=0;x<width;++x){const auto p=(std::size_t(y)*width+x)*3u;
            row[x*3u]=rgb[p+2];row[x*3u+1]=rgb[p+1];row[x*3u+2]=rgb[p];}
        out.write(reinterpret_cast<const char*>(row.data()),row.size());
    }
    return bool(out);
}
}

void UltimoSinal::PreviewTables(){
    benchmarkMode=true;sessionActive=false;hasSave=false;walking=false;
    investigation.Start();investigation.screen=ultimo::Screen::Explore;
    settings.quality=2;settings.brightness=1.f;settings.reduceMotion=true;
    saveNotice.clear();toastTime=0;target=-1;worldTime=0;duration=0;doorAmount=0;
    camera=CCamera({-.2f,1.55f,4.5f},{0,1,0},-140.f,-10.f);camera.Zoom=58;
    SyncCursor();
}

bool UltimoSinal::RenderReview(){
    const auto oldInvestigation=investigation;const auto oldAtmosphere=atmosphere;const auto oldExperience=experience;const auto oldCamera=camera;const auto oldSettings=settings;
    const auto oldAnswer=answer;const auto oldNotice=saveNotice;const auto oldDrama=drama;
    const bool oldBenchmark=benchmarkMode,oldHasSave=hasSave,oldWalking=walking;
    const float oldTime=worldTime,oldDoor=doorAmount,oldToast=toastTime;
    const int oldTarget=target,oldMenu=menuReturn,oldInspected=inspectedEvidence,oldPreview=previewEvidence;const float oldLensPlayback=lensPlaybackTime;const double oldDuration=duration;
    struct Restore{std::function<void()> run;~Restore(){run();}} restore{[&]{
        investigation=oldInvestigation;atmosphere=oldAtmosphere;experience=oldExperience;camera=oldCamera;settings=oldSettings;answer=oldAnswer;drama=oldDrama;
        benchmarkMode=oldBenchmark;hasSave=oldHasSave;walking=oldWalking;worldTime=oldTime;
        doorAmount=oldDoor;toastTime=oldToast;target=oldTarget;duration=oldDuration;
        inspectedEvidence=oldInspected;previewEvidence=oldPreview;lensPlaybackTime=oldLensPlayback;menuReturn=oldMenu;saveNotice=oldNotice;captured=false;SyncCursor();
    }};
    try{
        const auto folder=std::filesystem::path("../logs/deep_frames");
        std::filesystem::create_directories(folder);
        std::ofstream manifest(folder/"manifest.txt",std::ios::trunc);
        benchmarkMode=true;hasSave=false;walking=false;saveNotice.clear();toastTime=0;target=-1;drama=ultimo::Drama{};
        settings.brightness=1.f;settings.quality=2;settings.reduceMotion=true;
        investigation.Start();worldTime=0;doorAmount=0;duration=260;
        auto pose=[&](glm::vec3 position,float yaw,float pitch=0.f){camera=CCamera(position,{0,1,0},yaw,pitch);camera.Zoom=58;};
        bool presentationFrame=false;
        auto frame=[&](const char* name){
            int width,height;glfwGetFramebufferSize(glfwGetCurrentContext(),&width,&height);
            glViewport(0,0,width,height);
            for(int pass=0;pass<2;++pass){
                ImGui_ImplOpenGL3_NewFrame();ImGui_ImplGlfw_NewFrame();
                // Review is deterministic internal rendering, not an input replay.
                auto& io=ImGui::GetIO();io.AddMousePosEvent(-FLT_MAX,-FLT_MAX);
                for(int button=0;button<5;++button)io.AddMouseButtonEvent(button,false);
                ImGui::NewFrame();if(investigation.screen==ultimo::Screen::Explore)FindTarget();if(presentationFrame){glClearColor(.13f,.18f,.20f,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);RenderWorld();DrawInterface();DrawExperience(true);}else DrawScene(0);ImGui::Render();ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }
            const auto error=glGetError();if(error!=GL_NO_ERROR){std::cerr<<"Render review draw GL error="<<error<<std::endl;return false;}
            bool ok=WriteBackBufferBmp(folder/name,width,height);
            manifest<<name<<" "<<width<<"x"<<height<<" quality=2 brightness=1 screen="<<int(investigation.screen)
                <<" camera="<<camera.Position.x<<","<<camera.Position.y<<","<<camera.Position.z
                <<" yaw="<<camera.Yaw<<" pitch="<<camera.Pitch<<" exported="<<ok<<"\n";
            std::cout<<"Render review "<<name<<" exported="<<ok<<std::endl;return ok;
        };
        bool ok=true;
        investigation.screen=ultimo::Screen::Explore;pose({0,1.7f,17},-90,7);
        ok=frame("01_exterior.bmp")&&ok;
        pose({-1.5f,1.7f,12},-102,-4);ok=frame("07_front_oblique.bmp")&&ok;
        pose({0,1.7f,4},-90,-4);ok=frame("02_service_room.bmp")&&ok;
        if(mesa){
            // These views precede the electrical/key puzzle: the new furniture
            // must already be visible in the first accessible room.
            pose({-.2f,1.55f,4.5f},-140,-10);ok=frame("09_mesas_sala_inicial.bmp")&&ok;
            pose({1.5f,1.65f,4.1f},-45,-15);ok=frame("10_bancada_radio.bmp")&&ok;
        }
        auto face=[&](glm::vec3 position,glm::vec3 point){
            auto d=glm::normalize(point-position);
            pose(position,glm::degrees(std::atan2(d.z,d.x)),glm::degrees(std::asin(d.y)));
        };
        face({-1.7f,1.65f,3.4f},{-3,1.16f,3});ok=frame("11_diario_modelo.bmp")&&ok;
        face({-1.5f,1.25f,10.3f},{-2.65f,.30f,9});ok=frame("12_caixa_cais.bmp")&&ok;
        face({3.5f,1.70f,4.0f},{4.67f,1.55f,3.6f});ok=frame("13_painel_modelo.bmp")&&ok;
        face({-2.4f,1.65f,3.1f},{-3.7f,1.25f,4.15f});ok=frame("14_armario_fechado.bmp")&&ok;
        face({2.2f,1.7f,-1.4f},{0,2.6f,-3});ok=frame("19_escada_corrigida.bmp")&&ok;
        investigation.powerSolved=true;
        face({-2.4f,1.65f,3.1f},{-3.7f,1.25f,4.15f});ok=frame("15_armario_chave.bmp")&&ok;
        face({1.8f,1.7f,3.3f},{3,1.40f,2});ok=frame("16_radio_modelo.bmp")&&ok;
        face({-1.1f,1.0f,10.0f},{-2,.01f,9});ok=frame("17_marcas_modelo.bmp")&&ok;
        face({2.2f,1.7f,16.1f},{0,.25f,19});ok=frame("18_cais_modelo.bmp")&&ok;
        investigation.powerSolved=true;investigation.radioSolved=true;investigation.discovered.fill(true);doorAmount=1;
        pose({0,1.7f,-2},-90,-4);ok=frame("03_lens_room.bmp")&&ok;
        menuReturn=int(ultimo::Screen::Pause);investigation.screen=ultimo::Screen::Options;
        ok=frame("04_options.bmp")&&ok;
        investigation.screen=ultimo::Screen::Reading;investigation.readingEntry=0;investigation.readingReturn=ultimo::Screen::Explore;
        ok=frame("05_diary.bmp")&&ok;
        investigation.lensSolved=investigation.lensManual=true;investigation.lensLong.fill(true);investigation.screen=ultimo::Screen::Conclusion;investigation.hypothesis=1;
        investigation.evidence[7]=true;investigation.evidence[8]=true;
        const std::string argument="Brumas registrou a queda antes do comando manual. O registro e a fotografia relacionam os tres pulsos ao desembarque.";
        answer.fill(0);argument.copy(answer.data(),answer.size()-1);investigation.justification=answer.data();
        ok=frame("06_report.bmp")&&ok;
        if(mesa){
            investigation.screen=ultimo::Screen::Explore;target=-1;
            pose({1.55f,1.48f,-1.50f},-46,-12);
            ok=frame("08_mesa_importada.bmp")&&ok;
            manifest<<"Mesa uses imported UVs/material12, measured Y-up bounds and no additional axis rotation.\n";
        }
        // Deterministic presentation frames render the same native scene/UI at
        // declared animation times; they are not fabricated desktop screenshots.
        presentationFrame=true;investigation.Start();experience.Reset(investigation,false);
        investigation.screen=ultimo::Screen::Title;experience.menuTime=2.5f;
        face({6.6f,2.8f,13.45f},{-.7f,2.8f,-1});camera.Zoom=54;
        ok=frame("20_menu_cinematico.bmp")&&ok;
        experience.BeginIntro();experience.introTime=4.1f;investigation.screen=ultimo::Screen::Intro;
        face({0,2.2f,18.5f},{0,2.2f,1});camera.Zoom=54;
        ok=frame("21_prologo.bmp")&&ok;
        experience.CompleteIntro();investigation.screen=ultimo::Screen::Explore;
        pose({0,1.7f,17},-90,0);ok=frame("22_capitulo_chegada.bmp")&&ok;
        face({-2.4f,1.65f,3.1f},{-3.7f,1.25f,4.15f});
        ok=frame("23_armario_sem_energia.bmp")&&ok;
        investigation.powerSolved=true;experience.Observe(investigation,false);
        for(int i=0;i<7;++i)experience.Tick(.1f,true,false);
        ok=frame("24_armario_abrindo.bmp")&&ok;
        for(int i=0;i<9;++i)experience.Tick(.1f,true,false);
        ok=frame("25_armario_aberto.bmp")&&ok;
        investigation.discovered[3]=true;doorAmount=1;experience.Observe(investigation,true);experience.Tick(.1f,true,false);
        pose({0,1.7f,-2},-90,-4);ok=frame("26_capitulo_lente.bmp")&&ok;
        investigation.screen=ultimo::Screen::Radio;ok=frame("27_radio_interface.bmp")&&ok;
        investigation.powerSolved=investigation.radioSolved=true;investigation.discovered.fill(true);doorAmount=1;
        investigation.screen=ultimo::Screen::Explore;face({1.0f,8.9f,-4.5f},interactionPositions[12]);target=12;
        ok=frame("28_comando_modelo.bmp")&&ok;
        investigation.screen=ultimo::Screen::LensConsole;investigation.notice.clear();
        ok=frame("29_comando_interface.bmp")&&ok;
        investigation.lensSolved=investigation.lensManual=true;investigation.lensLong.fill(true);
        ok=frame("30_comando_confirmado.bmp")&&ok;
        investigation.screen=ultimo::Screen::Reading;investigation.readingEntry=1;inspectedEvidence=1;
        evidenceYaw=20;previewEvidence=1;evidencePitch=0;evidenceZoom=1;
        ok=frame("31_inspecao_alicate.bmp")&&ok;
        investigation.readingEntry=4;inspectedEvidence=4;
        ok=frame("32_inspecao_foto.bmp")&&ok;inspectedEvidence=-1;
        investigation.screen=ultimo::Screen::Journal;ok=frame("33_caderno_organizado.bmp")&&ok;
        investigation.screen=ultimo::Screen::Explore;experience.chapterAge=99;experience.activeEvent=ultimo::ExperienceEvent::None;
        pose({-.7f,1.65f,3.05f},-155,-8);ok=frame("34_armario_area_livre.bmp")&&ok;
        face({1.95f,1.7f,-1.4f},{0,2.5f,-3});ok=frame("35_escada_espiral.bmp")&&ok;
        face({2.1f,8.9f,-2.3f},{0,8.3f,-3});ok=frame("36_sala_optica.bmp")&&ok;
        face({1.4f,1.65f,3.7f},{3.2f,1.28f,2.5f});ok=frame("37_escrivaninha_radio.bmp")&&ok;
        face({-.8f,1.50f,2.4f},{-2.9f,1.15f,1.8f});ok=frame("38_diario_couro.bmp")&&ok;
        manifest<<"Exports are internal rendering fixtures. They do not certify manual play or participant tests.\n";
        // Phase art review, including a quiet letter moment and reduced motion.
        presentationFrame=true;investigation.screen=ultimo::Screen::Explore;
        experience.activeEvent=ultimo::ExperienceEvent::None;experience.chapterAge=ultimo::ChapterCardDuration;
        atmosphere=ultimo::Atmosphere{};settings.reduceMotion=false;worldTime=43.f;
        pose({0,1.7f,4.2f},-31,-8);ok=frame("39_arte_servico.bmp")&&ok;
        pose({1.9f,1.7f,-1.25f},-119,6);ok=frame("40_arte_arquivo.bmp")&&ok;
        investigation.lensSolved=false;pose({1.25f,8.9f,-4.30f},135,-9);ok=frame("41_arte_optica.bmp")&&ok;
        investigation.lensSolved=true;atmosphere.resolution=1.f;pose({1.25f,8.9f,-4.30f},135,-9);ok=frame("42_arte_optica_resolvida.bmp")&&ok;
        pose({1.7f,1.7f,3.5f},-34,-14);
        atmosphere.moment=ultimo::AtmosphereMoment::Letter;atmosphere.captionVisible=true;atmosphere.age=2.3f;atmosphere.quiet=1.f;
        ok=frame("43_momento_carta.bmp")&&ok;
        atmosphere.moment=ultimo::AtmosphereMoment::Optics;atmosphere.age=2.3f;
        pose({1.25f,8.9f,-4.30f},135,-9);ok=frame("44_chegada_optica.bmp")&&ok;
        atmosphere.captionVisible=false;settings.reduceMotion=true;
        pose({1.25f,8.9f,-4.30f},135,-9);ok=frame("45_movimento_reduzido.bmp")&&ok;
        // Presentation states of the 12/11 revision. Each value is set directly,
        // so the frame documents the look of a state, not a recorded session.
        settings.reduceMotion=false;atmosphere=ultimo::Atmosphere{};investigation.lensSolved=false;
        pose({0,1.7f,17},-90,7);drama.storm=1.f;ok=frame("46_relampago_chegada.bmp")&&ok;drama.storm=0;
        investigation.lensSolved=true;atmosphere.resolution=1.f;drama.lensFlash=1.f;
        pose({1.25f,8.9f,-4.30f},135,-9);ok=frame("47_ensaio_lente_pulso.bmp")&&ok;drama.lensFlash=0;
        drama.dawn=1.f;face({6.6f,2.8f,13.45f},{-.7f,2.8f,-1});ok=frame("48_ceu_apos_relatorio.bmp")&&ok;drama.dawn=0;
        face({-.8f,1.50f,2.4f},{-2.9f,1.15f,1.8f});ok=frame("49_mira_destaque.bmp")&&ok;
        settings.quality=0;pose({0,1.7f,4.2f},-31,-8);ok=frame("50_qualidade_baixa_direto.bmp")&&ok;settings.quality=2;
        const bool hadPost=postEnabled;postEnabled=false;
        pose({0,1.7f,4.2f},-31,-8);ok=frame("51_sem_pos_processamento.bmp")&&ok;postEnabled=hadPost;
        pose({0,1.7f,4.2f},-31,-8);ok=frame("52_com_pos_processamento.bmp")&&ok;
        manifest<<"Frames 46-52: storm fill, local lens glow, clearing sky, aim feedback and the direct/HDR paths.\n";
        return ok&&bool(manifest);
    }catch(const std::exception& error){std::cerr<<"Render review: "<<error.what()<<std::endl;return false;}
}
