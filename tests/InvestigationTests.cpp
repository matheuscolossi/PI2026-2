#include "../BaseAppOGL/Scenes/UltimoSinal/Investigation.h"
#include "../BaseAppOGL/Scenes/UltimoSinal/Geometry.h"
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
    Check(!game.FinishPrototype(),"nao concluir sem pistas");
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
            if(n<4)Check(!game.FinishPrototype(),"conclusao antecipada bloqueada");
        }
        game.ToggleJournal();Check(game.ReadAgain(ClueId::Diary),"reler descoberta");game.CloseReading();
        Check(game.FinishPrototype(),"cinco pistas encerram teste");
        Check(game.screen==Screen::Complete,"estado final");
        game.Start();Check(game.Count()==0 && !game.AllFound(),"reinicio apaga descobertas");
        ++permutations;
    }while(std::next_permutation(order.begin(),order.end()));
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
