#pragma once
#include <algorithm>
#include <cmath>

namespace ultimo {
struct Vec3 { float x,y,z; };
struct Bounds { Vec3 center,size; };
// Intersecao exata segmento/AABB (slabs), sem amostragem que ignore paredes finas.
inline bool SegmentHits(Vec3 from,Vec3 to,const Bounds& box) {
    const float origins[]{from.x,from.y,from.z};
    const float ends[]{to.x,to.y,to.z};
    const float centers[]{box.center.x,box.center.y,box.center.z};
    const float sizes[]{box.size.x,box.size.y,box.size.z};
    float enter=0.f,leave=1.f;
    for(int axis=0;axis<3;++axis) {
        float lo=centers[axis]-sizes[axis]*.5f,hi=centers[axis]+sizes[axis]*.5f;
        float direction=ends[axis]-origins[axis];
        if(std::abs(direction)<.000001f) {
            if(origins[axis]<lo || origins[axis]>hi)return false;
        } else {
            float a=(lo-origins[axis])/direction,b=(hi-origins[axis])/direction;
            if(a>b)std::swap(a,b);
            enter=(std::max)(enter,a);leave=(std::min)(leave,b);
            if(enter>leave)return false;
        }
    }
    return leave>=0 && enter<=1;
}
inline bool BlocksWalker(float x,float z,const Bounds& b,float radius=.25f) {
    return std::abs(x-b.center.x)<b.size.x*.5f+radius && std::abs(z-b.center.z)<b.size.z*.5f+radius;
}
}
