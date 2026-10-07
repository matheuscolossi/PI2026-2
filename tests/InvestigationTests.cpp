#include "../BaseAppOGL/Scenes/UltimoSinal/Investigation.h"
#include "../BaseAppOGL/Scenes/UltimoSinal/Geometry.h"
#include "../BaseAppOGL/Engine/MouseMotion.h"
#include "../BaseAppOGL/Scenes/UltimoSinal/Settings.h"
#include <limits>
#include <iostream>
#include <cstdlib>
#include <array>
#include <algorithm>
static int checks=0;
static void Check(bool ok,const char* message){++checks;if(!ok){std::cerr<<"FALHOU: "<<message<<'\n';std::exit(1);}}
int main(){
    using namespace ultimo;
    Investigation game;
    Check(game.screen==Screen::Title,"menu inicial");
    Check(!game.Inspect(ClueId::Key),"nao coletar no menu");
    game.Start();
    Check(!game.BeginConclusion(),"nao concluir sem pistas");
    Check(!game.Inspect(ClueId::Count),"rejeitar ID invalido");
    game.TogglePause();Check(!game.Inspect(ClueId::Key),"nao coletar em pausa");
    game.ToggleJournal();Check(game.screen==Screen::Pause,"caderno nao sobrepoe pausa");
    game.TogglePause();game.ToggleJournal();
    Check(!game.ReadAgain(ClueId::Photograph),"nao reler pista nao descoberta");
    game.ToggleJournal();
    std::array<int,5> order{0,1,2,3,4};int permutations=0;
    do {
        game.Start();Check(!game.DoorOpen(),"reinicio fecha passagem");
        for(int n=0;n<5;++n){
            auto id=static_cast<ClueId>(order[n]);
            Check(game.Inspect(id),"examinar pista");
            Check(game.Count()==std::size_t(n+1),"contagem unica");
            Check(game.DoorOpen()==game.Found(ClueId::Key),"somente chave abre passagem");
            Check(!game.Inspect(ClueId::Diary),"nao coletar durante leitura");
            game.CloseReading();game.Inspect(id);game.CloseReading();
            Check(game.Count()==std::size_t(n+1),"repeticao nao duplica");
            if(n<4)Check(!game.BeginConclusion(),"conclusao antecipada bloqueada");
        }
        game.ToggleJournal();Check(game.ReadAgain(ClueId::Diary),"reler descoberta");game.CloseReading();Check(game.screen==Screen::Journal,"releitura volta ao caderno");game.ToggleJournal();
        Check(game.BeginConclusion(),"cinco pistas encerram teste");
        Check(game.screen==Screen::Conclusion,"cinco pistas abrem formulario, nao feedback");
        Check(!game.Submit(),"sem alternativa e justificativa nao enviar");
        game.hypothesis=1;game.evidence[0]=true;game.justification="O diario registra ameacas.";
        Check(!game.Submit(),"uma evidencia nao basta");
        game.evidence[1]=true;game.justification="  \t\n";
        Check(!game.Submit(),"espacos nao sao justificativa");
        game.hypothesis=3;game.justification="A ameaca e o cabo cortado sugerem homicidio.";
        Check(!game.Submit(),"hipotese invalida bloqueada");game.hypothesis=1;
        game.TogglePause();game.TogglePause();Check(game.screen==Screen::Conclusion,"pausa restaura formulario");
        Check(game.Submit() && game.Correct(),"gabarito e feedback");
        Check(!game.Submit(),"nao reenviar depois de feedback");
        game.Start();Check(game.Count()==0 && !game.AllFound(),"reinicio apaga descobertas");
        ++permutations;
    }while(std::next_permutation(order.begin(),order.end()));
    Check(game.hypothesis==-1 && game.justification.empty() && std::count(game.evidence.begin(),game.evidence.end(),true)==0,"reinicio limpa relatorio");
    for(int alternative:{0,2}){
        game.Start();for(int i=0;i<5;++i){game.Inspect(static_cast<ClueId>(i));game.CloseReading();}
        game.BeginConclusion();game.hypothesis=alternative;game.evidence[0]=game.evidence[1]=true;game.justification="Resposta de teste";
        Check(game.Submit()&&!game.Correct(),"alternativa incorreta tambem conclui e recebe feedback");
    }
    MouseMotion motion;motion.Add(100,100);motion.Add(104,98);motion.Add(110,95);
    Check(motion.dx==10 && motion.dy==5,"somar todos os eventos do quadro");motion.Clear();
    Check(motion.dx==0 && motion.dy==0,"nao repetir eventos no proximo quadro");
    motion.Add(112,96);Check(motion.dx==2 && motion.dy==-1,"preservar origem entre quadros");
    motion.Reset();motion.Add(900,600);Check(motion.dx==0&&motion.dy==0,"recaptura nao causa salto");
    motion.Add(903,598);Check(motion.dx==3&&motion.dy==2,"movimento depois da recaptura");
    MouseMotion whole,split;whole.Add(0,0);split.Add(0,0);whole.Add(100,50);
    double totalX=0,totalY=0;
    for(int i=1;i<=100;++i){split.Add(i,i*.5);if(i%4==0){totalX+=split.dx;totalY+=split.dy;split.Clear();}}
    Check(totalX==whole.dx&&totalY==whole.dy,"mesma rotacao independente de divisao em quadros");
    Settings settings;settings.sensitivity=std::numeric_limits<float>::quiet_NaN();Check(settings.Sensitivity()==.06f,"NaN volta ao padrao");
    settings.sensitivity=20;Check(settings.Sensitivity()==.20f,"limite de sensibilidade");settings.Reset();Check(!settings.invertY&&settings.Sensitivity()==.06f,"restaurar controles");
    Bounds thin{{0,1,0},{.001f,2,3}};
    Check(SegmentHits({-2,1,0},{2,1,0},thin),"parede fina bloqueia mira");
    Check(SegmentHits({2,1,0},{-2,1,0},thin),"segmento inverso");
    Check(!SegmentHits({-2,3,0},{2,3,0},thin),"segmento acima da parede");
    Check(!SegmentHits({-2,1,4},{2,1,4},thin),"segmento ao lado da parede");
    Check(!SegmentHits({1,0,0},{1,2,0},thin),"paralelo fora do volume");
    Check(SegmentHits({0,1,0},{0,1,0},thin),"ponto dentro do volume");
    Bounds door{{0,1.35f,-.5f},{2,2.7f,.25f}};
    Check(BlocksWalker(0,-.5f,door),"porta bloqueia centro");
    Check(BlocksWalker(1.1f,-.5f,door),"raio do jogador considerado");
    Check(!BlocksWalker(1.3f,-.5f,door),"fora da porta livre");
    std::cout<<checks<<" verificacoes aprovadas; "<<permutations<<" ordens das pistas.\n";
}
