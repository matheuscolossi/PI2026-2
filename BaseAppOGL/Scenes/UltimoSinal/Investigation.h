#pragma once
#include "Narrative.h"
#include <algorithm>
#include <string>
#include <cctype>
namespace ultimo {
enum class Screen { Title, Explore, Reading, Journal, Pause, Conclusion, Complete };
class Investigation {
public:
    Screen screen=Screen::Title;
    ClueId reading=ClueId::Diary;
    int hypothesis=-1;
    std::array<bool,ClueCount> evidence{};
    std::string justification;
    void Start(){found.fill(false);evidence.fill(false);hypothesis=-1;justification.clear();screen=Screen::Explore;readingReturn=Screen::Explore;}
    std::size_t Count()const{return std::count(found.begin(),found.end(),true);}
    bool Found(ClueId id)const{auto i=static_cast<std::size_t>(id);return i<ClueCount && found[i];}
    bool DoorOpen()const{return Found(ClueId::Key);}
    bool AllFound()const{return Count()==ClueCount;}
    bool Inspect(ClueId id){
        auto i=static_cast<std::size_t>(id);if(screen!=Screen::Explore||i>=ClueCount)return false;
        found[i]=true;reading=id;readingReturn=Screen::Explore;screen=Screen::Reading;return true;
    }
    bool ReadAgain(ClueId id){
        if(screen!=Screen::Journal||!Found(id))return false;
        reading=id;readingReturn=Screen::Journal;screen=Screen::Reading;return true;
    }
    void CloseReading(){if(screen==Screen::Reading)screen=readingReturn;}
    void ToggleJournal(){if(screen==Screen::Explore)screen=Screen::Journal;else if(screen==Screen::Journal)screen=Screen::Explore;}
    void TogglePause(){
        if(screen==Screen::Pause)screen=pauseReturn;
        else if(screen==Screen::Explore||screen==Screen::Reading||screen==Screen::Journal||screen==Screen::Conclusion){pauseReturn=screen;screen=Screen::Pause;}
    }
    bool BeginConclusion(){if(screen!=Screen::Explore||!AllFound())return false;screen=Screen::Conclusion;return true;}
    bool CanSubmit()const{
        return screen==Screen::Conclusion && AllFound() && hypothesis>=0 && hypothesis<int(Hypotheses.size()) &&
            std::count(evidence.begin(),evidence.end(),true)>=2 &&
            std::any_of(justification.begin(),justification.end(),[](unsigned char c){return !std::isspace(c);});
    }
    bool Submit(){if(!CanSubmit())return false;screen=Screen::Complete;return true;}
    bool Correct()const{return screen==Screen::Complete && hypothesis==1;}
private:
    std::array<bool,ClueCount> found{};
    Screen readingReturn=Screen::Explore,pauseReturn=Screen::Explore;
};
}
