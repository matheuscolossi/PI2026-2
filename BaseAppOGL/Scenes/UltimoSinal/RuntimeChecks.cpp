#include "UltimoSinal.h"
#include <iostream>
#include <cmath>
bool UltimoSinal::VerifyRuntime(){
    int failures=0,checks=0;
    auto check=[&](bool ok,const char* label){++checks;std::cout<<(ok?"PASS ":"FAIL ")<<label<<std::endl;if(!ok)++failures;};
    Start();check(Blocked({0,1.7f,-.5f}),"porta trancada antes da chave");
    check(Blocked({5,1.7f,2}),"parede bloqueia jogador");
    check(!Visible(cluePositions[4]),"fotografia nao selecionavel atraves da porta");
    auto initial=camera.Position;Walk({1,0,-1},.1f);
    check(std::abs(glm::length(camera.Position-initial)-.3f)<.0001f,"velocidade diagonal normalizada");Start();
    auto travel=[&](glm::vec3 destination){
        for(int i=0;i<1500 && glm::length(destination-camera.Position)>.035f;++i){auto v=destination-camera.Position;Walk(v,(std::min)(1.f/60.f,glm::length(v)/3.f));}
        check(glm::length(destination-camera.Position)<.04f,"percurso por movimento e colisoes");
    };
    auto inspect=[&](ultimo::ClueId id,glm::vec3 from){
        travel(from);auto d=glm::normalize(cluePositions[std::size_t(id)]-camera.Position);
        camera=CCamera(camera.Position,{0,1,0},glm::degrees(std::atan2(d.z,d.x)),glm::degrees(std::asin(d.y)));camera.Zoom=58;
        FindTarget();check(target==int(id),"mira seleciona pista real no alcance sem obstrucao");
        if(target==int(id))investigation.Inspect(id);
        check(investigation.Found(id),"pista registrada");investigation.CloseReading();
    };
    inspect(ultimo::ClueId::Footprints,{0,1.7f,10});
    inspect(ultimo::ClueId::Key,{.7f,1.7f,8.2f});
    check(!Blocked({0,1.7f,-.5f}),"chave libera a passagem");
    travel({0,1.7f,3.8f});inspect(ultimo::ClueId::Diary,{1.8f,1.7f,2});
    travel({0,1.7f,2});inspect(ultimo::ClueId::Tool,{-1.8f,1.7f,3});
    travel({0,1.7f,2});travel({0,1.7f,-2});inspect(ultimo::ClueId::Photograph,{1.8f,1.7f,-3});
    check(investigation.BeginConclusion(),"formulario liberado apos percurso");
    investigation.hypothesis=1;investigation.evidence[0]=investigation.evidence[1]=true;investigation.justification="Teste tecnico: ameacas e cabo cortado. Nao e resposta de participante.";
    check(investigation.Submit()&&investigation.Correct(),"percurso chega ao feedback");
    Start();check(investigation.Count()==0&&Blocked({0,1.7f,-.5f}),"reinicio restaura pistas e porta");
    std::cout<<"Runtime geometry: "<<checks<<" checks; failures="<<failures<<std::endl;
    return failures==0;
}

void UltimoSinal::BenchmarkPose(float time){
    benchmarkMode=true;investigation.screen=ultimo::Screen::Explore;
    float phase=std::fmod(time,40.f)/40.f;float z=17.f-20.f*(1.f-std::abs(2.f*phase-1.f));
    camera=CCamera({0,1.7f,z},{0,1,0},-90.f+50.f*std::sin(time*.3f),-5.f+5.f*std::cos(time*.15f));camera.Zoom=58;
    if(!investigation.DoorOpen()){investigation.Inspect(ultimo::ClueId::Key);investigation.CloseReading();}
}
