#include "UltimoSinal.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <string>
#include <filesystem>
#include <unordered_set>
#include <string_view>

using ultimo::Screen;
namespace {
constexpr float Pi=3.14159265359f;
const glm::vec3 Chalk(.72f,.73f,.65f), Paint(.20f,.31f,.30f), Rust(.37f,.20f,.12f);
const glm::vec3 Iron(.12f,.16f,.17f), Wood(.38f,.24f,.13f), Paper(.77f,.72f,.57f);
glm::vec3 V(ultimo::Vec3 v){return {v.x,v.y,v.z};}
}

void UltimoSinal::LoadWorldAssets(){
    if(worldAssetsScanned)return;worldAssetsScanned=true;
    const char* names[]={"ilha","costa","costalod","caminho","cais","carga","caisbox","anexo","piso","torre","porta","cadeira","caneca","armario","armporta","botas","escada","radio","quadro","botao","mapa","papel","relogio","arquivo","lumin","lampada","diario","alicate","marcas","chave","foto","agua","janvid","vidfar","led","fragment","radioluz","escrivan","decor"};
    for(const char* name:names){
        auto path=std::string("Scenes/Models/Cenario/")+name+".3ds";std::error_code error;
        if(!std::filesystem::is_regular_file(path,error))continue;
        auto model=std::make_unique<CAsset>(path,false);const auto& report=model->GetReport();
        const bool valid=report.error.empty()&&report.hasBounds&&report.triangleCount>0&&report.vertexCount>0
            &&report.positionsFinite&&report.normalsFinite&&report.normalFallbackVertices==0
            &&report.uvsFinite&&report.meshesWithUVs==report.meshCount&&report.uvFallbackVertices==0
            &&report.missingTextures.empty();
        if(valid){
            if(std::string(name)=="piso")floorUsesCompleteMap=std::any_of(report.diffuseTexturePaths.begin(),report.diffuseTexturePaths.end(),[](const std::string& texture){
                // 3DS exports bitmap names in uppercase; Windows resolves both.
                auto leaf=std::filesystem::path(texture).filename().string();
                std::transform(leaf.begin(),leaf.end(),leaf.begin(),[](unsigned char c){return char(std::tolower(c));});
                return leaf=="p_bake.jpg";
            });
            std::cout<<"Max world asset accepted: "<<name<<" triangles="<<report.triangleCount<<std::endl;worldAssets.emplace(name,std::move(model));
        }
        else{
            std::cerr<<"Max world asset rejected; procedural fallback retained: "<<name
                <<" error='"<<report.error<<"' meshes="<<report.meshCount<<" vertices="<<report.vertexCount<<" triangles="<<report.triangleCount
                <<" bounds="<<report.hasBounds<<" positionsFinite="<<report.positionsFinite
                <<" normalsFinite="<<report.normalsFinite<<" normalFallback="<<report.normalFallbackVertices
                <<" uvsFinite="<<report.uvsFinite<<" meshesWithUVs="<<report.meshesWithUVs<<" uvFallback="<<report.uvFallbackVertices
                <<" missingTextures="<<report.missingTextures.size()<<std::endl;
            for(const auto& missing:report.missingTextures)std::cerr<<"  missing texture: "<<missing<<std::endl;
        }
    }
    std::cout<<"Max world library: "<<worldAssets.size()<<"/39 assets; missing assets retain their own fallback."<<std::endl;
    std::cout<<"Max floor bake: CompleteMap UV1 applied="<<floorUsesCompleteMap<<std::endl;
}

void UltimoSinal::BuildWorld(){
    blocks.clear();
    auto solid=[&](glm::vec3 p,glm::vec3 s){blocks.push_back({{{p.x,p.y,p.z},{s.x,s.y,s.z}},Chalk,true});};
    // Every large prop and wall has a matching volume. Glass also blocks walking.
    solid({-5,1.6f,-.5f},{.30f,3.2f,11});solid({5,1.6f,-.5f},{.30f,3.2f,11});
    solid({0,1.6f,-6},{10,3.2f,.30f});
    for(float x:{-3.f,3.f}){solid({x,1.6f,5},{4,3.2f,.30f});solid({x,1.6f,-.5f},{4,3.2f,.30f});}
    if(mesa){for(const auto& desk:desks)blocks.push_back({desk.bounds,Wood,true});}
    else{
        for(auto p:{glm::vec3(-3,.52f,3),glm::vec3(-3,.52f,1),glm::vec3(-3,.52f,-3),glm::vec3(3,.52f,-3)})solid(p,{1.65f,1.04f,.95f});
        solid({3,.52f,2.5f},{1.65f,1.04f,1.95f});
    }
    solid({-3.7f,1.05f,4.45f},{1.25f,2.10f,.60f}); // key is in front of collider
    solid({-3.7f,.58f,4.15f},{1.25f,1.16f,1.15f}); // shelf footprint, below key sightline
    solid({0,7.80f,-4.90f},{.60f,1.0f,.53f}); // upper optical console pedestal
    solid({4.6f,.48f,-4.65f},{.55f,.96f,1.4f});
    solid({-3.7f,.35f,12.3f},{1.5f,.7f,1.15f});solid({4.6f,.46f,8.2f},{1.35f,.92f,1.25f});
    solid({-2.65f,.20f,9},{.55f,.40f,.55f});
    for(float z:{2.f,3.2f})solid({4.30f,.45f,z},{.55f,.90f,.55f});
}

void UltimoSinal::Render(CPrimitives& primitive,glm::vec3 center,glm::vec3 size,glm::vec3 color,int material,float alpha){
    shader.SetMat4("model",glm::scale(glm::translate(glm::mat4(1),center),size));
    shader.SetVec4("Color",glm::vec4(color,alpha));shader.SetInt("material",material);primitive.render();
}

void UltimoSinal::DrawWorld(){
    LoadWorldAssets();
    int width,height;glfwGetFramebufferSize(glfwGetCurrentContext(),&width,&height);if(benchmarkMode){width=SCR_WIDTH;height=SCR_HEIGHT;}if(width<=0||height<=0)return;
    const auto projection=glm::perspective(glm::radians(camera.Zoom),float(width)/height,.1f,240.f);
    lastProjection=projection;
    const int quality=std::clamp(settings.quality,0,2);
    glEnable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_BLEND);
    shader.Use("Prototype");shader.SetMat4("view",camera.GetViewMatrix());shader.SetMat4("projection",projection);
    shader.SetVec3("eye",camera.Position);shader.SetVec3("look",camera.Front);
    const float visualTime=settings.reduceMotion?0.f:experience.VisualTime(worldTime);
    shader.SetFloat("time",visualTime);shader.SetFloat("brightness",std::clamp(settings.brightness,.75f,1.35f));
    shader.SetFloat("powerPulse",settings.reduceMotion?0.f:experience.powerPulse);
    shader.SetFloat("resolutionWarmth",benchmarkMode?(investigation.lensSolved?1.f:0.f):atmosphere.resolution);
    shader.SetBool("rainMotion",!settings.reduceMotion);
    shader.SetInt("quality",quality);shader.SetBool("powered",investigation.powerSolved);
    shader.SetBool("useNormal",normalEnabled&&quality>0);shader.SetBool("useFog",fogEnabled);
    // The hand torch belongs to the player; title and prologue cameras carry none.
    const bool cinematicCamera=IsMenuBackdrop()||investigation.screen==Screen::Intro;
    shader.SetBool("usePoint",pointEnabled);shader.SetBool("useSpot",spotEnabled&&!cinematicCamera);
    shader.SetBool("useEnvironment",environmentEnabled);shader.SetBool("useMulti",multiEnabled);
    // Presentation state: linear HDR output when the post passes are active,
    // storm fill, lamp stutter, local lens glow, clearing sky and aim feedback.
    shader.SetBool("hdrOutput",post.active);shader.SetFloat("emissiveBoost",3.4f);
    shader.SetFloat("storm",drama.storm);shader.SetFloat("lensFlash",drama.lensFlash);
    shader.SetFloat("dawn",drama.dawn);shader.SetFloat("lampSurge",drama.lampSurge);
    {
        // Same footprints FindTarget uses for aiming, so the glow matches the prop.
        constexpr std::array<float,13> reach{{.32f,.24f,.60f,.16f,.28f,.29f,.60f,0.f,.29f,.29f,.50f,.44f,.40f}};
        const float breathe=settings.reduceMotion?.82f:.74f+.18f*std::sin(worldTime*3.1f);
        const bool aimed=target>=0&&target<int(reach.size())&&investigation.screen==Screen::Explore;
        shader.SetVec4("focusPoint",glm::vec4(focusPosition,focusFade*breathe));
        shader.SetFloat("focusRadius",aimed?reach[target]*.85f+.10f:.35f);
    }
    for(int i=0;i<3;++i){glActiveTexture(GL_TEXTURE0+i);textures.ApplyTexture(i);}
    shader.SetInt("grassTex",0);shader.SetInt("stoneTex",1);shader.SetInt("normalTex",2);
    glActiveTexture(GL_TEXTURE3);glBindTexture(GL_TEXTURE_CUBE_MAP,skybox->GetSkyboxTexture());shader.SetInt("environment",3);

    // Analytical box shadows. Unlike walking barriers, these leave window gaps.
    // They approximate architectural/furniture volumes; they are not a Max bake.
    std::vector<ultimo::Bounds> occluders;
    auto occlude=[&](glm::vec3 p,glm::vec3 s){occluders.push_back({{p.x,p.y,p.z},{s.x,s.y,s.z}});};
    for(float x:{-5.f,5.f}){
        occlude({x,.59f,-.5f},{.3f,1.18f,11});occlude({x,2.9f,-.5f},{.3f,.6f,11});
        for(auto v:{glm::vec2(-5.175f,1.65f),glm::vec2(-.75f,3.8f),glm::vec2(3.925f,2.15f)})occlude({x,1.9f,v.x},{.3f,1.4f,v.y});
    }
    occlude({0,1.6f,-6},{10,3.2f,.3f});
    for(float x:{-3.f,3.f}){occlude({x,1.6f,5},{4,3.2f,.3f});occlude({x,1.6f,-.5f},{4,3.2f,.3f});}
    // The actual ceiling has an aperture matching the spiral staircase.
    for(float x:{-3.375f,3.375f})occlude({x,3.15f,-.5f},{3.65f,.12f,11.4f});
    occlude({0,3.15f,1.875f},{3.1f,.12f,6.65f});occlude({0,3.15f,-5.375f},{3.1f,.12f,1.65f});
    if(mesa){for(const auto& desk:desks){auto center=V(desk.bounds.center),size=V(desk.bounds.size);center.y+=size.y*.5f-.06f;occlude(center,{size.x,.12f,size.z});}}
    else{
        for(auto p:{glm::vec3(-3,1,3),glm::vec3(-3,1,1),glm::vec3(-3,1,-3),glm::vec3(3,1,-3)})occlude(p,{1.65f,.13f,.95f});
        occlude({3,1,2.5f},{1.65f,.13f,1.95f});
    }
    if(doorAmount<.15f)occluders.push_back(door);
    const int count=quality==0?0:int((std::min)(occluders.size(),std::size_t(32)));shader.SetInt("blockerCount",count);
    if(count>0){
        // Conservative union of exactly the boxes sent to the shader. Rebuilt
        // each frame, so moving/opening doors cannot leave a stale broad phase.
        glm::vec3 unionMin=V(occluders.front().center)-V(occluders.front().size)*.5f;
        glm::vec3 unionMax=V(occluders.front().center)+V(occluders.front().size)*.5f;
        for(int i=1;i<count;++i){const auto& b=occluders[i];
            unionMin=(glm::min)(unionMin,V(b.center)-V(b.size)*.5f);
            unionMax=(glm::max)(unionMax,V(b.center)+V(b.size)*.5f);
        }
        // A small outward margin avoids rejecting edge rays due to rounding.
        shader.SetVec3("shadowBoundsMin",unionMin-glm::vec3(.001f));
        shader.SetVec3("shadowBoundsMax",unionMax+glm::vec3(.001f));
    }
    for(int i=0;i<count;++i){const auto& b=occluders[i];shader.SetVec3("blockerMin["+std::to_string(i)+"]",V(b.center)-V(b.size)*.5f);shader.SetVec3("blockerMax["+std::to_string(i)+"]",V(b.center)+V(b.size)*.5f);}
    auto box=[&](glm::vec3 p,glm::vec3 s,glm::vec3 c,int m=0){Render(cube,p,s,c,m);};
    auto rotated=[&](CPrimitives& primitive,glm::vec3 p,glm::vec3 s,glm::vec3 color,float degrees,glm::vec3 axis,int m=0,float alpha=1.f){
        auto model=glm::rotate(glm::translate(glm::mat4(1),p),glm::radians(degrees),axis);
        shader.SetMat4("model",glm::scale(model,s));shader.SetVec4("Color",glm::vec4(color,alpha));shader.SetInt("material",m);primitive.render();
    };
    auto beam=[&](glm::vec3 a,glm::vec3 b,float radius,glm::vec3 color,int mat=0){
        glm::vec3 d=b-a;float len=glm::length(d);if(len<.0001f)return;d/=len;
        glm::vec3 axis=glm::cross(glm::vec3(0,1,0),d);if(glm::length(axis)<.001f)axis={1,0,0};else axis=glm::normalize(axis);
        rotated(cylinder,(a+b)*.5f,{radius,len,radius},color,glm::degrees(std::acos(std::clamp(d.y,-1.f,1.f))),axis,mat);
    };
    const glm::mat4 identity(1.f);
    // Track requested model groups, not hidden objects or technical renderers.
    // A complete library must also draw without returning to placeholder meshes.
    std::unordered_set<std::string> nativeFallbacks;
    auto lookup=[&](const char* name)->CAsset*{
        auto found=worldAssets.find(name);
        if(found==worldAssets.end()){nativeFallbacks.emplace(name);return nullptr;}
        return found->second.get();
    };
    auto at=[&](glm::vec3 position){return glm::translate(identity,position);};
    auto placed=[&](glm::vec3 position,glm::vec3 scale){return glm::scale(at(position),scale);};
    auto native=[&](const char* name,const glm::mat4& model,int material=12,glm::vec4 tint=glm::vec4(1)){
        auto asset=lookup(name);if(!asset)return false;
        shader.SetMat4("model",model);shader.SetInt("material",material);shader.SetVec4("Color",tint);
        // Surface response changes highlights, never the authored bitmap/UVs.
        const std::string_view key(name);int surface=0;
        if(key=="escrivan"||key=="porta"||key=="armario"||key=="armporta"||key=="cais"||key=="carga"||key=="cadeira")surface=1;
        else if(key=="radio"||key=="quadro"||key=="arquivo"||key=="lumin"||key=="caisbox"||key=="alicate"||key=="chave")surface=2;
        else if(key=="anexo"||key=="torre")surface=3;
        else if(key=="papel"||key=="mapa"||key=="foto"||key=="diario")surface=4;
        else if(key=="caminho"||key=="escada"||key=="costa"||key=="costalod")surface=5;
        shader.SetInt("surfaceClass",surface);
        AssetDrawOptions options;
        // Dynamic emission, glass and water are driven by the actual game state.
        if(material==4||material==5||material==6){options.useMaterialColor=false;options.useMaterialOpacity=false;}
        asset->Draw(shader.GetProgram("Prototype"),options);return true;
    };
    auto supported=[&](const char* name,float x,float z,float surface,int material=12,glm::vec4 tint=glm::vec4(1)){
        auto asset=lookup(name);if(!asset)return false;
        float y=surface+.001f-asset->GetReport().boundsMin.y;
        return native(name,at({x,y,z}),material,tint);
    };
    auto fitted=[&](const char* name,const glm::mat4& parent,glm::vec3 center,glm::vec3 dimensions,int material,glm::vec4 tint){
        auto asset=lookup(name);if(!asset)return false;
        const auto& report=asset->GetReport();auto size=report.boundsMax-report.boundsMin;
        if(size.x<.000001f||size.y<.000001f||size.z<.000001f){nativeFallbacks.emplace(name);return false;}
        auto local=glm::translate(identity,center)*glm::scale(identity,dimensions/size)*glm::translate(identity,-(report.boundsMin+report.boundsMax)*.5f);
        return native(name,parent*local,material,tint);
    };
    const float deskTop=mesa?desks.front().bounds.center.y+desks.front().bounds.size.y*.5f:1.05f;

    // The irregular coast contains the entire safe rectangular walking region.
    if(!native("ilha",identity,1))Render(waterPlane,{0,-.025f,0},{36,1,56},{.80f,.84f,.77f},1);
    const int rocks=quality==0?28:48;
    bool coastReady=quality==0?(native("costalod",identity,2)||native("costa",identity,2)):native("costa",identity,2);
    if(coastReady){nativeFallbacks.erase("costalod");nativeFallbacks.erase("costa");}
    if(!coastReady){
    for(int i=0;i<rocks;++i){
        float a=i*2*Pi/rocks,c=std::cos(a),s=std::sin(a),v=.8f+.22f*std::sin(i*7.12f);
        float x=15.5f*std::copysign(std::pow(std::abs(c),1.f/3),c),z=24.f*std::copysign(std::pow(std::abs(s),1.f/3),s);
        rotated(cylinder,{x,-.48f,z},{1.5f*v,.92f,1.9f*v},{.38f,.40f,.37f},i*43.f,{.3f,1,.5f},2);
        if(quality>0)rotated(cone,{x+.4f,-.22f,z+.3f},{1.1f*v,.9f,1.2f},{.43f,.44f,.39f},18,{1,0,1},2);
    }
    }
    if(!native("caminho",identity,2))for(int j=0;j<13;++j)box({.07f*std::sin(j*2.f),.009f,5.6f+j*.9f},{1.6f+.13f*std::sin(float(j)),.06f,.78f},{.45f,.43f,.35f},2);
    if(!native("cais",identity)){
    for(int j=0;j<10;++j)box({0,.07f,17.5f+j*.35f},{2.15f,.14f,.31f},Wood,3);
    for(float x:{-.83f,.83f})box({x,-.08f,19},{.16f,.28f,3.55f},Wood*.72f,3);
    for(float x:{-1.20f,1.20f})for(float z:{17.45f,20.6f}){
        Render(cylinder,{x,.28f,z},{.13f,1.45f,.13f},Wood*.65f,3);Render(cylinder,{x,.89f,z},{.16f,.08f,.16f},Iron);
        if(quality>0)for(int k=0;k<3;++k)Render(cylinder,{x,.65f+k*.06f,z},{.16f,.035f,.16f},{.43f,.36f,.23f},3);
    }
    }
    for(auto p:{glm::vec3(-3.7f,.35f,12.3f),glm::vec3(4.6f,.46f,8.2f)}){
        auto sz=p.x<0?glm::vec3(1.5f,.7f,1.15f):glm::vec3(1.35f,.92f,1.25f);
        if(fitted("carga",identity,p,sz,12,glm::vec4(1)))continue;
        box(p,sz,Wood*.8f,3);
        for(float x:{-.44f,.44f})box(p+glm::vec3(x,0,sz.z*.505f),{.07f,sz.y+.03f,.04f},Iron);
        box(p+glm::vec3(0,.04f,sz.z*.52f),{.42f,.25f,.02f},Paper,9);
    }
    // Observable condition of the blue dock circuit, paired with maintenance.
    if(!native("caisbox",identity)){
    box({-2.65f,.16f,9},{.55f,.32f,.55f},{.16f,.30f,.47f},11);
    box({-2.65f,.33f,9},{.45f,.03f,.44f},Iron);
    rotated(cube,{-2.65f,.47f,9.20f},{.57f,.055f,.55f},{.16f,.30f,.47f},-60,{1,0,0},11);
    for(int j=0;j<3;++j){box({-2.81f+j*.16f,.37f,9},{.035f,.06f,.15f},{.66f,.61f,.43f},11);box({-2.82f+j*.16f,.35f,9.09f},{.09f,.018f,.08f},Paper*.9f);}
    beam({-2.65f,.06f,8.75f},{-2.4f,.06f,5.5f},.018f,{.12f,.26f,.44f},10);
    beam({-2.4f,.06f,5.5f},{4.85f,.06f,5.5f},.018f,{.12f,.26f,.44f},10);
    beam({4.85f,.06f,5.5f},{5.22f,1.45f,3.6f},.018f,{.12f,.26f,.44f},10);
    }

    // Annex: limewash above a sea-green dado, foundation, framed window openings.
    // Foundation top (.08) stays below the wood floor (.095). Its four outer
    // edges project .05 beyond the walls in X/Z, avoiding coplanar wall faces.
    // Native CompleteMap already contains static illumination and shadows.
    // Material 13 displays its UV1 atlas without applying dynamic light again.
    // The single-node 3DS export stores floor geometry about its own pivot.
    // Restore its authored height/centre so the foundation cannot cover the bake.
    if(!native("piso",at({0,.075f,-.5f}),floorUsesCompleteMap?13:12))box({0,.075f,-.5f},{9.7f,.04f,10.7f},{.29f,.24f,.17f},3);
    if(!native("anexo",identity)){
    box({0,0,-.5f},{10.40f,.16f,11.40f},{.42f,.40f,.34f},2);
    for(float x:{-5.f,5.f}){
        box({x,.59f,-.5f},{.3f,1.18f,11},Paint,7);box({x,2.9f,-.5f},{.3f,.6f,11},Chalk,7);
        for(auto v:{glm::vec2(-5.175f,1.65f),glm::vec2(-.75f,3.8f),glm::vec2(3.925f,2.15f)})box({x,1.9f,v.x},{.3f,1.4f,v.y},Chalk,7);
        for(float z:{-3.5f,2.f}){
            for(float zz:{z-.88f,z+.88f})box({x,1.9f,zz},{.40f,1.6f,.10f},Iron);
            for(float y:{1.18f,2.62f})box({x,y,z},{.40f,.10f,1.85f},Iron);
            box({x,1.9f,z},{.35f,.06f,1.75f},Iron);box({x,1.10f,z},{.60f,.10f,1.95f},{.37f,.37f,.30f},2);
        }
        box({x,.12f,-.5f},{.46f,.24f,11.25f},{.39f,.40f,.35f},2);
    }
    for(float z:{-6.f,-.5f,5.f}){
        if(z==-6){box({0,1.6f,z},{10,3.2f,.3f},Chalk,7);box({0,.58f,z+.17f},{9.8f,1.16f,.025f},Paint,7);}
        else{
            for(float x:{-3.f,3.f}){box({x,1.6f,z},{4,3.2f,.3f},Chalk,7);box({x,.58f,z+(z>0?-.17f:.17f)},{4,1.16f,.025f},Paint,7);}
            box({0,2.95f,z},{2,.5f,.3f},Chalk,7);
            for(float x:{-1.06f,1.06f})box({x,1.37f,z},{.14f,2.75f,.44f},Wood*.75f,3);
            box({0,2.76f,z},{2.28f,.14f,.44f},Wood*.75f,3);
        }
    }
    // Pitched roof and rain gutters, separate from the interior ceiling.
    // The ceiling trim shares the foundation's .05 projection on both axes.
    box({0,3.15f,-.5f},{10.40f,.12f,11.40f},{.28f,.26f,.20f},3);
    for(float x:{-2.65f,2.65f})rotated(cube,{x,3.87f,-.5f},{5.55f,.12f,11.8f},Rust,x<0?16.f:-16.f,{0,0,1},8);
    box({0,4.63f,-.5f},{.22f,.15f,11.95f},Iron,8);
    for(float z:{-6.33f,5.32f})for(float x:{-5.25f,5.25f}){box({x,3.17f,z},{.14f,.15f,.6f},Iron);Render(cylinder,{x,1.6f,z},{.045f,3.2f,.045f},Iron);}
    }
    // Recognizable tower silhouette, gallery railings, stepped Fresnel lens.
    if(!native("torre",identity)){
    Render(cylinder,{0,3.65f,-3},{2.38f,.8f,2.38f},{.53f,.53f,.45f},2);
    Render(cylinder,{0,5.48f,-3},{1.93f,3.1f,1.93f},Chalk,7);Render(cylinder,{0,5.88f,-3},{1.95f,.72f,1.95f},Rust,7);
    Render(cylinder,{0,7.10f,-3},{2.48f,.22f,2.48f},Iron,8);
    for(int i=0;i<16;++i){
        float a=i*2*Pi/16,b=(i+1)*2*Pi/16;glm::vec3 p(2.28f*std::cos(a),7.62f,-3+2.28f*std::sin(a)),q(2.28f*std::cos(b),7.62f,-3+2.28f*std::sin(b));
        beam(p-glm::vec3(0,.42f,0),p+glm::vec3(0,.42f,0),.028f,Iron);beam(p+glm::vec3(0,.35f,0),q+glm::vec3(0,.35f,0),.025f,Iron);
        if(quality>0)beam(p-glm::vec3(0,.12f,0),q-glm::vec3(0,.12f,0),.018f,Iron);
    }
    Render(cylinder,{0,7.35f,-3},{1.62f,.22f,1.62f},Iron);
    for(int i=0;i<8;++i){float a=i*2*Pi/8;Render(cylinder,{1.53f*std::cos(a),8.10f,-3+1.53f*std::sin(a)},{.045f,1.45f,.045f},Iron);}
    for(int j=0;j<7;++j)Render(cylinder,{0,7.68f+j*.13f,-3},{.65f,.035f,.65f},{.43f,.54f,.48f},11);
    Render(cylinder,{0,8.85f,-3},{1.75f,.18f,1.75f},Iron,8);Render(cone,{0,9.38f,-3},{2.02f,.94f,2.02f},Rust,8);Render(cylinder,{0,10.03f,-3},{.035f,.55f,.035f},Iron);
    }
    const float lensBreath=settings.reduceMotion?1.f:.985f+.015f*std::sin(visualTime*.57f);
    // Before any investigation exists, the title backdrop shows the lit optic
    // of the case's premise. It is a cover image, not a state of the session.
    const bool opticLit=investigation.powerSolved||(IsMenuBackdrop()&&!sessionActive);
    const glm::vec3 lensColor=opticLit?lensBreath*glm::vec3(.98f,.61f,.23f):glm::vec3(.38f,.42f,.39f);
    if(!native("lampada",placed({0,8.10f,-3},{.38f,1.05f,.38f}),opticLit?6:12,glm::vec4(lensColor,1)))
        Render(cylinder,{0,8.10f,-3},{.38f,1.05f,.38f},lensColor,opticLit?6:0);

    // Hinged leaf: unlocking animates the physical object instead of hiding it.
    {
        auto model=glm::rotate(glm::translate(glm::mat4(1),glm::vec3(-.97f,1.35f,-.5f)),glm::radians(-100.f*doorAmount),glm::vec3(0,1,0));
        if(!native("porta",model)){
        auto leaf=[&](glm::vec3 offset,glm::vec3 size,glm::vec3 color,int mat){shader.SetMat4("model",glm::scale(glm::translate(model,offset),size));shader.SetVec4("Color",glm::vec4(color,1));shader.SetInt("material",mat);cube.render();};
        leaf({.97f,0,0},{1.94f,2.7f,.13f},Wood*.75f,3);
        for(float y:{-.65f,.40f})leaf({.97f,y,-.074f},{1.48f,.70f,.025f},Paint,7);
        leaf({1.73f,-.12f,.10f},{.20f,.045f,.10f},{.56f,.45f,.24f},11);
        }
    }
    auto table=[&](glm::vec3 p,float depth){
        box(p+glm::vec3(0,.99f,0),{1.65f,.12f,depth},Wood,3);box(p+glm::vec3(0,.83f,0),{1.47f,.18f,depth-.18f},Wood*.7f,3);
        for(float x:{-.68f,.68f})for(float z:{-depth*.36f,depth*.36f})box(p+glm::vec3(x,.45f,z),{.10f,.90f,.10f},Wood*.65f,3);
        if(quality>0)box(p+glm::vec3(0,.36f,0),{1.40f,.07f,.10f},Wood*.65f,3);
    };
    if(!mesa){
        nativeFallbacks.emplace("mesa");
        for(auto p:{glm::vec3(-3,0,3),glm::vec3(-3,0,1),glm::vec3(-3,0,-3),glm::vec3(3,0,-3)})table(p,.95f);
        table({3,0,2.5f},1.95f);
    }
    // Two empty chairs face the sea; the cold cup corroborates the optional letter.
    for(float z:{2.f,3.2f}){
        if(native("cadeira",at({4.30f,.095f,z})))continue;
        box({4.30f,.48f,z},{.55f,.10f,.55f},Wood*.7f,3);
        for(float x:{4.08f,4.52f})for(float zz:{z-.22f,z+.22f})box({x,.24f,zz},{.055f,.48f,.055f},Iron);
        for(float zz:{z-.22f,z+.22f})box({4.06f,.74f,zz},{.05f,.54f,.05f},Iron);
        box({4.06f,.88f,z},{.07f,.23f,.55f},Paint,7);
    }
    const float deskLift=mesa?.03f:0.f;
    auto onDesk=[&](glm::vec3 p){p.y+=deskLift;return p;};
    if(!supported("caneca",3.52f,3.08f,deskTop)){
    Render(cylinder,onDesk({3.52f,1.16f,3.08f}),{.085f,.18f,.085f},Paper,9);
    Render(cylinder,onDesk({3.52f,1.253f,3.08f}),{.068f,.006f,.068f},{.08f,.06f,.04f},10);
    beam(onDesk({3.59f,1.21f,3.08f}),onDesk({3.65f,1.21f,3.08f}),.015f,Paper);beam(onDesk({3.65f,1.21f,3.08f}),onDesk({3.65f,1.12f,3.08f}),.015f,Paper);beam(onDesk({3.65f,1.12f,3.08f}),onDesk({3.59f,1.12f,3.08f}),.015f,Paper);
    }
    // Open cabinet with a projecting shelf: key lies in a recess, not on the route.
    if(!native("armario",at({-3.7f,0,4.15f}))){
    box({-3.7f,1.05f,4.73f},{1.25f,2.1f,.08f},Paint,7);
    for(float x:{-4.32f,-3.08f})box({x,1.05f,4.15f},{.08f,2.1f,1.15f},Paint,7);
    for(float y:{.08f,1.20f,2.08f})box({-3.7f,y,4.15f},{1.25f,.08f,1.15f},Wood*.7f,3);
    box({-3.7f,1.65f,4.67f},{.95f,.56f,.025f},Iron);
    for(int j=0;j<4;++j)beam({-4.04f+j*.22f,1.73f,4.65f},{-4.04f+j*.22f,1.73f,4.48f},.013f,{.58f,.46f,.22f},11);
    }
    {
        const float cabinet=(benchmarkMode||settings.reduceMotion)?(investigation.powerSolved?1.f:0.f):experience.cabinetAmount;
        const float angle=110.f*cabinet;
        auto frame=glm::rotate(glm::translate(glm::mat4(1),glm::vec3(-4.32f,1.05f,3.55f)),glm::radians(angle),glm::vec3(0,1,0));
        auto leaf=[&](glm::vec3 offset,glm::vec3 size,glm::vec3 color,int material){shader.SetMat4("model",glm::scale(glm::translate(frame,offset),size));shader.SetVec4("Color",glm::vec4(color,1));shader.SetInt("material",material);cube.render();};
        if(!native("armporta",frame)){
        leaf({.62f,0,0},{1.24f,2.1f,.06f},Paint,11);
        leaf({1.06f,0,-.05f},{.04f,.22f,.04f},Iron,0);
        leaf({.58f,.68f,-.04f},{.60f,.13f,.025f},Paper,9);
        }
        const glm::vec3 indicator=investigation.powerSolved?glm::vec3(.35f,.72f,.42f):glm::vec3(.66f,.26f,.14f);
        if(!fitted("led",frame,{1.05f,.40f,-.04f},{.07f,.07f,.025f},6,glm::vec4(indicator,1)))leaf({1.05f,.40f,-.04f},{.07f,.07f,.025f},indicator,6);
    }
    if(!native("botas",at({-1.685f,.095f,4.3f})))for(float x:{-1.85f,-1.52f}){box({x,.24f,4.40f},{.22f,.45f,.25f},{.11f,.09f,.06f},10);box({x,.095f,4.21f},{.23f,.17f,.33f},{.11f,.09f,.06f},10);}
    // Native full-height spiral, authored at the tower's real world position.
    native("escada",identity);
    native("decor",identity);

    // Radio, panel and chart are identifiable stations; essential text is in UI.
    glm::mat4 radioModel=at(onDesk({3,1.26f,2}));
    if(auto found=worldAssets.find("radio");found!=worldAssets.end())radioModel=at({3,deskTop+.001f-found->second->GetReport().boundsMin.y,2});
    if(!native("radio",radioModel)){
    box(onDesk({3,1.26f,2}),{.80f,.39f,.36f},Paint,11);box(onDesk({3,1.27f,2.19f}),{.66f,.23f,.02f},Iron);
    for(float x:{3.13f,3.27f})rotated(cylinder,onDesk({x,1.23f,2.23f}),{.044f,.05f,.044f},Iron,90,{1,0,0});
    beam(onDesk({2.65f,1.46f,1.94f}),onDesk({2.46f,2.11f,1.94f}),.012f,Iron);
    }
    const glm::vec3 radioColor=investigation.radioSolved?glm::vec3(.36f,.80f,.53f):glm::vec3(.47f,.51f,.41f);
    if(!native("radioluz",radioModel*at({-.17f,.067f,.216f}),6,glm::vec4(radioColor,1)))box(onDesk({2.79f,1.31f,2.21f}),{.25f,.065f,.018f},radioColor,6);
    if(!native("quadro",at({4.67f,1.55f,3.6f}))){
    box({4.67f,1.55f,3.6f},{.23f,.92f,.74f},Paint,11);box({4.53f,1.55f,3.6f},{.035f,.75f,.60f},Iron);
    for(int j=0;j<3;++j)box({4.50f,1.40f+j*.16f,3.74f},{.045f,.04f,.05f},Paper,9);
    }
    for(int j=0;j<3;++j){
        const auto position=glm::vec3(4.50f,1.40f+j*.16f,3.50f);
        const auto color=investigation.breakers[j]?(investigation.powerSolved?glm::vec3(.42f,.63f,.43f):glm::vec3(.61f,.40f,.17f)):glm::vec3(.25f,.28f,.27f);
        auto transform=glm::scale(glm::rotate(at(position),glm::radians(investigation.breakers[j]?-16.f:16.f),glm::vec3(0,0,1)),glm::vec3(.04f,.08f,.20f));
        if(!native("botao",transform,12,glm::vec4(color,1)))box(position,{.04f,.08f,.20f},color,11);
    }
    if(worldAssets.find("anexo")==worldAssets.end()){
        beam({4.72f,2,3.6f},{4.72f,2.88f,3.6f},.022f,Iron);beam({4.72f,2.88f,3.6f},{.1f,2.88f,3.6f},.022f,Iron);
    }
    // Chart is accessible in room one, on the INSIDE of the front wall.
    if(!native("mapa",at({1.70f,1.53f,4.78f}))){
    box({1.70f,1.53f,4.78f},{1.23f,.96f,.07f},Wood*.65f,3);box({1.70f,1.53f,4.73f},{1.10f,.83f,.015f},Paper,9);
    for(int j=0;j<5;++j){box({1.70f,1.20f+j*.16f,4.715f},{1.01f,.006f,.008f},{.34f,.43f,.42f});box({1.26f+j*.21f,1.53f,4.714f},{.006f,.76f,.008f},{.34f,.43f,.42f});}
    for(int j=0;j<8;++j){float a=j*2*Pi/8;rotated(cube,{1.70f+.28f*std::cos(a),1.53f+.20f*std::sin(a),4.704f},{.26f,.022f,.014f},Paint,j*45.f,{0,0,1});}
    box({1.80f,1.69f,4.69f},{.035f,.035f,.02f},Rust);
    }
    // Small motivated desk bulb; its shade and stem belong to the native decor.
    native("lampada",placed({3.48f,1.69f,3.02f},{.07f,.07f,.07f}),6,glm::vec4(.94f,.64f,.27f,1));
    const float documentY=mesa?desks.front().bounds.center.y+desks.front().bounds.size.y*.5f+.011f:1.065f;
    for(auto p:{glm::vec3(-2.5f,documentY,.25f),glm::vec3(3,documentY,3),glm::vec3(-3,documentY,-3)}){
        if(supported("papel",p.x,p.z,deskTop))continue;
        box(p,{.47f,.015f,.32f},Paper,9);for(int j=0;j<4;++j)box(p+glm::vec3(-.04f,.011f,-.1f+j*.045f),{.28f,.002f,.008f},{.26f,.29f,.25f});
    }
    // Reuse a native panel instance as a local bench instrument. Normalize its
    // measured bounds before rotating X-facing hardware toward the room (+Z).
    // The paper remains in front, separately targetable; the desk blocks walking.
    if(auto instrument=worldAssets.find("quadro");instrument!=worldAssets.end()){
        const auto& report=instrument->second->GetReport();
        auto size=report.boundsMax-report.boundsMin;
        auto model=at({0,8.48f,-4.90f})*glm::rotate(identity,glm::radians(90.f),glm::vec3(0,1,0))
            *glm::scale(identity,glm::vec3(.18f,.44f,.54f)/size)*at(-(report.boundsMin+report.boundsMax)*.5f);
        native("quadro",model);
        for(int j=0;j<3;++j){
            auto switchModel=glm::rotate(at({-.18f+j*.18f,8.47f,-4.79f}),glm::radians(investigation.lensLong[j]?-16.f:16.f),glm::vec3(1,0,0));
            native("botao",glm::scale(switchModel,glm::vec3(.06f,.10f,.04f)),12,glm::vec4(.55f,.52f,.39f,1));
        }
        bool pulse=false;
        if(lensPlaybackTime>=ultimo::LensLeadIn){
            const float t=lensPlaybackTime-ultimo::LensLeadIn;
            pulse=t<2.f*ultimo::LensPulseSpacing+ultimo::LensPulseDuration&&std::fmod(t,ultimo::LensPulseSpacing)<ultimo::LensPulseDuration;
        }
        if(settings.reduceMotion)pulse=lensPlaybackTime>=0;
        const glm::vec3 lamp=pulse?glm::vec3(.87f,.71f,.32f):investigation.lensSolved?glm::vec3(.20f,.39f,.30f):glm::vec3(.11f,.16f,.14f);
        native("led",glm::scale(at({.18f,8.60f,-4.79f}),glm::vec3(.035f)),6,glm::vec4(lamp,1));
    }
    if(!native("relogio",at({-2.3f,2.27f,-5.75f}))){
    rotated(cylinder,{-2.3f,2.27f,-5.78f},{.30f,.07f,.30f},Iron,90,{1,0,0});rotated(cylinder,{-2.3f,2.27f,-5.73f},{.26f,.012f,.26f},Paper,90,{1,0,0},9);
    beam({-2.3f,2.27f,-5.70f},{-2.3f,2.45f,-5.70f},.008f,Iron);beam({-2.3f,2.27f,-5.70f},{-2.16f,2.32f,-5.70f},.008f,Iron);
    }
    if(!native("arquivo",at({4.60f,.095f,-4.65f}))){
    box({4.60f,.48f,-4.65f},{.55f,.96f,1.4f},Paint,11);
    for(int j=0;j<3;++j){box({4.30f,.18f+j*.30f,-4.65f},{.035f,.24f,1.28f},Iron);box({4.27f,.18f+j*.30f,-4.65f},{.04f,.035f,.24f},Paper*.75f,11);}
    }
    for(float z:{2.5f,-3.f}){
        // A restrained pivot at the ceiling, not a camera shake. The bulb is
        // transformed with its shade so the two pieces cannot drift apart.
        const float sway=.55f*std::sin(visualTime*.63f+z);
        const glm::vec3 lampAnchor(z<0?2.4f:0.f,3.14f,z<0?-1.8f:z);
        const auto fixture=glm::rotate(at(lampAnchor),glm::radians(sway),glm::vec3(0,0,1));
        if(!native("lumin",fixture)){beam(lampAnchor,lampAnchor-glm::vec3(0,.33f,0),.015f,Iron);Render(cone,lampAnchor-glm::vec3(0,.31f,0),{.30f,.18f,.30f},Iron);}
        const auto bulb=fixture*glm::scale(at({0,-.41f,0}),glm::vec3(.16f,.04f,.16f));
        const glm::vec3 bulbColor=(investigation.powerSolved?1.f:.64f)*glm::vec3(.93f,.68f,.31f);
        if(!native("lampada",bulb,6,glm::vec4(bulbColor,1)))Render(cylinder,{0,2.73f,z},{.16f,.04f,.16f},bulbColor,6);
    }

    for(std::size_t i=0;i<cluePositions.size();++i){
        auto p=cluePositions[i];bool focused=int(i)==target&&investigation.screen==Screen::Explore;
        // Keep the interaction point unchanged while resting the paper on the
        // measured imported top, instead of hovering above the old placeholder.
        if(mesa){
            const auto& support=desks[i==0?0:i==1?1:3].bounds;
            const float top=support.center.y+support.size.y*.5f;
            if(i==0)p.y=top+.056f; // book half-height .055 plus contact clearance
            if(i==1)p.y=top+.0285f; // lowest tool fragment/handle is .0275 below origin
            if(i==4)p.y=top+.007f;
        }
        const glm::vec3 brass=focused?glm::vec3(.86f,.65f,.29f):glm::vec3(.55f,.43f,.23f);
        if(i==0){
            if(supported("diario",p.x,p.z,deskTop))continue;
            box(p,{.56f,.11f,.40f},{.21f,.12f,.08f},3);box(p+glm::vec3(0,.063f,0),{.50f,.018f,.35f},Paper,9);
            for(int j=0;j<5;++j)box(p+glm::vec3(.01f,.076f,-.11f+j*.046f),{.34f,.003f,.009f},{.22f,.25f,.22f});
            box(p+glm::vec3(-.24f,.075f,0),{.018f,.015f,.35f},Rust);
        }else if(i==1){
            const bool toolReady=supported("alicate",p.x,p.z,deskTop);
            const bool fragmentReady=supported("fragment",p.x+.24f,p.z-.12f,deskTop);
            if(!toolReady){
            for(float x:{-.09f,.09f}){rotated(cube,p+glm::vec3(x,0,.05f),{.065f,.055f,.33f},Rust,x<0?12.f:-12.f,{0,1,0},10);rotated(cube,p+glm::vec3(x*.4f,.012f,-.18f),{.065f,.06f,.17f},brass,x<0?-26.f:26.f,{0,1,0},11);}
            }
            if(!fragmentReady)box(p+glm::vec3(.24f,-.01f,-.12f),{.045f,.035f,.07f},{.41f,.43f,.39f},11);
        }else if(i==2){
            if(supported("marcas",p.x,p.z,-.025f))continue;
            // Partial wet marks are evidence, not an arrow toward the key.
            for(int j=0;j<3;++j){auto q=p+glm::vec3(j%2?.17f:-.15f,.018f,(j-1)*.48f);rotated(cube,q,{.15f,.008f,.24f},{.15f,.18f,.15f},j*17.f-12.f,{0,1,0},10);if(quality>0)for(int k=0;k<3;++k)box(q+glm::vec3(0,.006f,k*.05f-.06f),{.13f,.003f,.014f},{.23f,.25f,.20f});}
        }else if(i==3){
            if(investigation.DoorOpen())continue;
            if(supported("chave",p.x,p.z,1.24f))continue;
            Render(cylinder,p,{.067f,.025f,.067f},brass,11);box(p+glm::vec3(.11f,0,0),{.18f,.025f,.035f},brass,11);box(p+glm::vec3(.17f,0,.026f),{.04f,.025f,.08f},brass,11);
        }else if(i==4){
            if(supported("foto",p.x,p.z,deskTop))continue;
            for(float x:{-.15f,.15f}){auto q=p+glm::vec3(x,0,x<0?-.025f:.025f);box(q,{.27f,.012f,.38f},Paper,9);box(q+glm::vec3(0,.009f,0),{.23f,.004f,.31f},{.25f,.32f,.31f});box(q+glm::vec3(0,.013f,-.01f),{.22f,.004f,.07f},{.55f,.53f,.42f});box(q+glm::vec3(.02f,.016f,-.065f),{.09f,.005f,.07f},{.62f,.62f,.52f});}
        }
    }
    // Imported meshes share the vertex layout and lighting, but use their real
    // bitmap UVs. CAsset uses reserved units 4/5/6 and restores global bindings.
    if(mesa){
        shader.SetVec4("Color",glm::vec4(1));shader.SetInt("material",12);shader.SetInt("uDiffuseMap",0);shader.SetInt("surfaceClass",1);
        AssetDrawOptions options;
        for(std::size_t i=0;i<desks.size();++i){const auto& desk=desks[i];
            if(i==4){fitted("escrivan",identity,{3,.595f,2.5f},{1.6f,1.f,1.95f},12,glm::vec4(1));continue;}
            shader.SetMat4("model",desk.model);mesa->Draw(shader.GetProgram("Prototype"),options);
        }
    }
    shader.Use("SkyBox");shader.SetMat4("view",glm::mat4(glm::mat3(camera.GetViewMatrix())));shader.SetMat4("projection",projection);shader.SetInt("skyboxTextures",0);shader.SetFloat("brightness",settings.brightness);shader.SetFloat("time",visualTime);
    shader.SetBool("hdrOutput",post.active);shader.SetFloat("storm",drama.storm);shader.SetFloat("clearing",drama.dawn);skybox->DrawSkybox();glDisable(GL_CULL_FACE);
    shader.Use("Prototype");for(int i=0;i<3;++i){glActiveTexture(GL_TEXTURE0+i);textures.ApplyTexture(i);}glActiveTexture(GL_TEXTURE3);glBindTexture(GL_TEXTURE_CUBE_MAP,skybox->GetSkyboxTexture());
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glDepthMask(GL_FALSE);
    if(!native("agua",placed({0,-.40f,0},{220,1,220}),4,glm::vec4(.09f,.19f,.21f,.96f)))Render(waterPlane,{0,-.40f,0},{220,1,220},{.09f,.19f,.21f},4,.96f);
    if(!native("agua",placed({-2.65f,.014f,9},{.94f,1,.85f}),4,glm::vec4(.11f,.17f,.17f,.28f)))Render(waterPlane,{-2.65f,.014f,9},{.94f,1,.85f},{.11f,.17f,.17f},4,.28f);
    // Thin glass panes, rather than overlapping faces of a closed alpha cube.
    for(float x:{-5.f,5.f})for(float z:{-3.5f,2.f}){
        auto model=glm::scale(glm::rotate(at({x,1.90f,z}),glm::radians(90.f),glm::vec3(0,0,1)),glm::vec3(1.38f,1,1.68f));
        if(!native("janvid",model,5,glm::vec4(.49f,.68f,.68f,.14f)))rotated(waterPlane,{x,1.90f,z},{1.38f,1,1.68f},{.49f,.68f,.68f},90,{0,0,1},5,.14f);
    }
    glEnable(GL_CULL_FACE);glCullFace(GL_BACK);
    if(!native("vidfar",identity,5,glm::vec4(.64f,.83f,.78f,.16f)))Render(cylinder,{0,8.10f,-3},{1.52f,1.40f,1.52f},{.64f,.83f,.78f},5,.16f);
    glDisable(GL_CULL_FACE);
    // Soft cards approximate scattering in coastal mist. This is deliberately
    // not described as volumetric ray tracing or a shadow-map contribution.
    // The beam appears only after actual power restoration and freezes with
    // the world clock in a pause screen or with reduced motion enabled.
    if(opticLit){
        const glm::vec3 origin(0,8.10f,-3);
        const glm::vec3 direction=glm::normalize(glm::vec3(std::cos(visualTime*.18f+.47f),-.14f,std::sin(visualTime*.18f+.47f)));
        const auto midpoint=origin+direction*29.5f;
        auto side=glm::cross(direction,camera.Position-midpoint);
        side=glm::length(side)>.001f?glm::normalize(side):glm::normalize(glm::cross(direction,glm::vec3(0,1,0)));
        glm::mat4 frame(1.f);frame[0]=glm::vec4(side,0);frame[1]=glm::vec4(glm::cross(direction,side),0);frame[2]=glm::vec4(direction,0);frame[3]=glm::vec4(midpoint,1);
        shader.SetMat4("model",glm::scale(frame,glm::vec3(7.2f,1,56.f)));
        shader.SetVec4("Color",glm::vec4(.92f,.74f,.39f,.105f));shader.SetInt("material",14);waterPlane.render();
        // The rotating optic throws a second, fainter arm on the opposite side.
        if(quality>0){
            const auto opposite=glm::vec3(-direction.x,direction.y,-direction.z);
            const auto tip=origin+opposite*29.5f;
            auto across=glm::cross(opposite,camera.Position-tip);
            across=glm::length(across)>.001f?glm::normalize(across):glm::normalize(glm::cross(opposite,glm::vec3(0,1,0)));
            glm::mat4 arm(1.f);arm[0]=glm::vec4(across,0);arm[1]=glm::vec4(glm::cross(opposite,across),0);arm[2]=glm::vec4(opposite,0);arm[3]=glm::vec4(tip,1);
            shader.SetMat4("model",glm::scale(arm,glm::vec3(7.2f,1,56.f)));
            shader.SetVec4("Color",glm::vec4(.92f,.74f,.39f,.060f));shader.SetInt("material",14);waterPlane.render();
        }
    }
    // Only a few warm motes in the lamp pools, with no collision or clue ID.
    // No camera displacement, random flashes, or changed puzzle conditions.
    if(quality>0){
        const int motes=quality==2?30:18;
        auto fraction=[](float value){return value-std::floor(value);};
        for(int i=0;i<motes;++i){
            const float seed=fraction(std::sin(float(i)*12.9898f+4.12f)*43758.5453f);
            const float room=i%2==0?2.5f:-3.f;
            glm::vec3 position(1.75f*std::sin(i*4.71f+visualTime*.041f),
                .55f+2.05f*fraction(seed+visualTime*.016f),room+1.3f*std::cos(i*7.31f+visualTime*.035f));
            if(i%3==0&&camera.Position.y>6.f)position={1.65f*std::sin(i*4.71f+visualTime*.041f),7.6f+.95f*fraction(seed+visualTime*.016f),-3.f+1.65f*std::cos(i*7.31f)};
            if(glm::length(camera.Position-position)>9.f)continue;
            glm::mat4 facing(1.f);facing[0]=glm::vec4(camera.Right,0);facing[1]=glm::vec4(-camera.Front,0);facing[2]=glm::vec4(camera.Up,0);facing[3]=glm::vec4(position,1);
            shader.SetMat4("model",glm::scale(facing,glm::vec3(.018f+seed*.015f,1,.018f+seed*.015f)));
            shader.SetVec4("Color",glm::vec4(.74f,.66f,.46f,investigation.powerSolved?.095f:.045f));shader.SetInt("material",15);waterPlane.render();
        }
    }
    // Soft halos read the fixtures as light sources; the bloom pass widens
    // them. They are camera-facing cards, nudged forward to clear the ceiling.
    if(quality>0){
        const float supply=(investigation.powerSolved?1.f:.48f)*(std::max)(0.f,1.f+drama.lampSurge);
        auto halo=[&](glm::vec3 position,float size,glm::vec3 color,float opacity){
            const auto toEye=camera.Position-position;const float distance=glm::length(toEye);
            if(distance<.6f||distance>42.f)return;
            position+=toEye/distance*(std::min)(.30f,distance*.25f);
            glm::mat4 facing(1.f);facing[0]=glm::vec4(camera.Right,0);facing[1]=glm::vec4(-camera.Front,0);facing[2]=glm::vec4(camera.Up,0);facing[3]=glm::vec4(position,1);
            shader.SetMat4("model",glm::scale(facing,glm::vec3(size,1,size)));
            shader.SetVec4("Color",glm::vec4(color,opacity));shader.SetInt("material",15);waterPlane.render();
        };
        halo({0,2.73f,2.5f},.90f,{.98f,.67f,.31f},.22f*supply);
        halo({2.4f,2.73f,-1.8f},.90f,{.80f,.80f,.78f},.18f*supply);
        halo({3.48f,1.69f,3.02f},.52f,{1.f,.62f,.26f},.30f*supply);
        if(opticLit)halo({0,8.10f,-3},2.4f,{1.f,.66f,.27f},.20f+.30f*drama.lensFlash);
    }
    // Window shafts are depth-tested scattering cards, not new architecture
    // or true volumetric light. They remain within their own room.
    if(quality>0&&fogEnabled&&std::abs(camera.Position.x)<5.2f&&camera.Position.y<3.2f){
        for(float z:{2.f,-3.5f}){
            const glm::vec3 origin(4.90f,2.28f,z),direction=glm::normalize(glm::vec3(-1,-.32f,.12f));
            const auto midpoint=origin+direction*2.2f;auto side=glm::cross(direction,camera.Position-midpoint);
            if(glm::length(side)<.001f)continue;side=glm::normalize(side);
            glm::mat4 frame(1);frame[0]=glm::vec4(side,0);frame[1]=glm::vec4(glm::cross(direction,side),0);frame[2]=glm::vec4(direction,0);frame[3]=glm::vec4(midpoint,1);
            shader.SetMat4("model",glm::scale(frame,glm::vec3(1.15f,1,4.4f)));
            shader.SetVec4("Color",glm::vec4(.43f,.62f,.74f,.012f));shader.SetInt("material",14);waterPlane.render();
        }
    }
    // Rain falls only outside: no particles through the roof or in the shaft.
    // Reduced motion removes falling streaks; moisture on glass remains static.
    if(quality>0&&!settings.reduceMotion){
        const int drops=quality==2?132:66;
        auto fraction=[](float v){return v-std::floor(v);};
        for(int i=0;i<drops;++i){
            float a=fraction(std::sin(i*12.9898f+2.1f)*43758.5453f),b=fraction(std::sin(i*7.812f+8.2f)*19341.231f);
            glm::vec3 p(-9.f+a*18.f,12.f*fraction(b-visualTime*.36f),-10.f+b*25.f);
            if(std::abs(p.x)<5.7f&&p.z<5.6f&&p.z> -6.6f)continue;
            const float distance=glm::length(p-camera.Position);if(distance>18.f)continue;
            glm::vec3 side=camera.Right;side.y=0;if(glm::length(side)<.001f)continue;side=glm::normalize(side);
            glm::mat4 frame(1);frame[0]=glm::vec4(side,0);frame[1]=glm::vec4(glm::cross(glm::vec3(0,1,0),side),0);frame[2]=glm::vec4(0,1,0,0);frame[3]=glm::vec4(p,1);
            shader.SetMat4("model",glm::scale(frame,glm::vec3(.012f,1,.34f)));
            shader.SetVec4("Color",glm::vec4(.52f,.67f,.73f,.13f*(1.f-distance/18.f)));shader.SetInt("material",16);waterPlane.render();
        }
    }
    glDisable(GL_CULL_FACE);glDepthMask(GL_TRUE);glDisable(GL_BLEND);glActiveTexture(GL_TEXTURE0);
    if(!worldCoverageLogged){
        worldCoverageLogged=true;
        std::cout<<"Max native coverage: accepted="<<worldAssets.size()<<" fallback="<<nativeFallbacks.size()<<std::endl;
        for(const auto& name:nativeFallbacks)std::cout<<"Max native fallback group: "<<name<<std::endl;
    }
}
