#pragma once
#include <array>
#include <algorithm>
#include <cstddef>

namespace ultimo {
enum class ClueId : std::size_t { Diary, Tool, Footprints, Key, Photograph, Count };
enum class Screen { Title, Explore, Reading, Journal, Pause, Complete };
constexpr std::size_t ClueCount = static_cast<std::size_t>(ClueId::Count);
struct ClueText { const char* name; const char* description; };
// Conteudo deliberadamente provisório: nao representa um gabarito aprovado.
inline constexpr std::array<ClueText, ClueCount> ClueTexts{{
    {"Diario do faroleiro", "Um diario foi deixado sobre a mesa. O registro definitivo sera escrito depois da revisao do misterio."},
    {"Ferramenta quebrada", "Uma ferramenta danificada foi encontrada na primeira sala. Sua relacao com o ultimo sinal ainda sera definida."},
    {"Pegadas molhadas", "Marcas molhadas atravessam o caminho ate a entrada. Observe onde elas estao e compare com as demais pistas."},
    {"Chave da sala da lente", "A chave permite acessar a segunda sala. A passagem central agora esta liberada."},
    {"Fotografia rasgada", "Uma fotografia foi encontrada na sala da lente. Seu conteudo sera definido junto com o gabarito."}
}};

class Investigation {
public:
    Screen screen = Screen::Title;
    ClueId reading = ClueId::Diary;
    void Start() { found.fill(false); screen=Screen::Explore; }
    std::size_t Count() const { return std::count(found.begin(),found.end(),true); }
    bool Found(ClueId id) const {
        auto i=static_cast<std::size_t>(id);return i<ClueCount && found[i];
    }
    bool DoorOpen() const { return Found(ClueId::Key); }
    bool AllFound() const { return Count()==ClueCount; }
    bool Inspect(ClueId id) {
        auto i=static_cast<std::size_t>(id);
        if(screen!=Screen::Explore || i>=ClueCount)return false;
        found[i]=true;reading=id;screen=Screen::Reading;return true;
    }
    bool ReadAgain(ClueId id) {
        if(screen!=Screen::Journal || !Found(id))return false;
        reading=id;screen=Screen::Reading;return true;
    }
    void CloseReading() { if(screen==Screen::Reading)screen=Screen::Explore; }
    void ToggleJournal() {
        if(screen==Screen::Explore)screen=Screen::Journal;
        else if(screen==Screen::Journal)screen=Screen::Explore;
    }
    void TogglePause() {
        if(screen==Screen::Pause)screen=Screen::Explore;
        else if(screen==Screen::Explore || screen==Screen::Reading || screen==Screen::Journal)screen=Screen::Pause;
    }
    bool FinishPrototype() {
        if(screen!=Screen::Explore || !AllFound())return false;
        screen=Screen::Complete;return true;
    }
private:
    std::array<bool,ClueCount> found{};
};
}
