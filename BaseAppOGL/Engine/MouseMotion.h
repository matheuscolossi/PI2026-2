#pragma once
#include <cmath>
// Soma eventos entre quadros; Reset descarta a primeira amostra apos recaptura/foco.
struct MouseMotion {
    double x=0,y=0,dx=0,dy=0; bool first=true;
    void Reset(){dx=dy=0;first=true;}
    void Clear(){dx=dy=0;}
    void Add(double px,double py){
        if(!std::isfinite(px)||!std::isfinite(py)){Reset();return;}
        if(!first){dx+=px-x;dy+=y-py;}
        x=px;y=py;first=false;
    }
};
