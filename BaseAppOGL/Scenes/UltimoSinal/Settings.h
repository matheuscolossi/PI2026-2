#pragma once
#include <algorithm>
#include <cmath>
namespace ultimo {
struct Settings {
    float sensitivity=0.06f; bool invertY=false;
    void Reset(){sensitivity=.06f;invertY=false;}
    float Sensitivity()const{return std::isfinite(sensitivity)?(std::clamp)(sensitivity,.015f,.20f):.06f;}
};
}
