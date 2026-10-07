#include "UltimoSinal.h"
#include <cmath>
using ultimo::Screen;
static glm::vec3 V(ultimo::Vec3 v){return {v.x,v.y,v.z};}
void UltimoSinal::Render(CPrimitives& primitive,glm::vec3 center,glm::vec3 size,glm::vec3 color,int material,float alpha){
    shader.SetMat4("model",glm::scale(glm::translate(glm::mat4(1),center),size));
    shader.SetVec4("Color",glm::vec4(color,alpha));shader.SetInt("material",material);primitive.render();
}
void UltimoSinal::DrawWorld(){
    int width,height;glfwGetFramebufferSize(glfwGetCurrentContext(),&width,&height);if(width<=0||height<=0)return;
    auto projection=glm::perspective(glm::radians(camera.Zoom),float(width)/height,.1f,200.f);
    glEnable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_BLEND);
    shader.Use("Prototype");shader.SetMat4("view",camera.GetViewMatrix());shader.SetMat4("projection",projection);
    shader.SetVec3("eye",camera.Position);shader.SetVec3("look",camera.Front);shader.SetFloat("time",float(glfwGetTime()));
    shader.SetBool("useNormal",normalEnabled);shader.SetBool("useFog",fogEnabled);shader.SetBool("usePoint",pointEnabled);shader.SetBool("useSpot",spotEnabled);shader.SetBool("useEnvironment",environmentEnabled);shader.SetBool("useMulti",multiEnabled);
    for(int i=0;i<3;++i){glActiveTexture(GL_TEXTURE0+i);textures.ApplyTexture(i);}
    shader.SetInt("grassTex",0);shader.SetInt("stoneTex",1);shader.SetInt("normalTex",2);
    glActiveTexture(GL_TEXTURE3);glBindTexture(GL_TEXTURE_CUBE_MAP,skybox->GetSkyboxTexture());shader.SetInt("environment",3);
    // Physical volumes stay in the controller; furniture rendering uses legs and tops.
    for(std::size_t i=1;i<blocks.size();++i){
        const auto& b=blocks[i];auto c=V(b.bounds.center),s=V(b.bounds.size);
        if(i==2)continue; // path is now blended into the terrain
        if(i>=13){Render(cube,{c.x,.98f,c.z},{1.6f,.12f,.9f},{.36f,.23f,.13f},3);
            for(float x:{-.65f,.65f})for(float z:{-.32f,.32f})Render(cube,{c.x+x,.5f,c.z+z},{.10f,.96f,.10f},{.22f,.14f,.09f},3);
        }else Render(cube,c,s,i==1?glm::vec3(.9f):b.color,i==1?1:(i==3?3:2));
    }
    // Lighthouse: stone shaft, painted belt, gallery, lantern supports and roof.
    Render(cylinder,{0,5.1f,-3},{2.0f,4.f,2.0f},{.78f,.77f,.68f},2);
    Render(cylinder,{0,6.1f,-3},{2.03f,.65f,2.03f},{.28f,.15f,.12f});
    Render(cylinder,{0,7.18f,-3},{2.65f,.18f,2.65f},{.18f,.22f,.23f});
    for(int i=0;i<16;++i){float a=float(i)*6.2831853f/16;
        Render(cylinder,{2.48f*std::cos(a),7.65f,-3+2.48f*std::sin(a)},{.035f,.85f,.035f},{.13f,.17f,.18f});
    }
    Render(cylinder,{0,8.f,-3},{.22f,.9f,.22f},{1,.75f,.37f},6);
    for(int i=0;i<8;++i){float a=float(i)*6.2831853f/8;
        Render(cylinder,{1.57f*std::cos(a),8.f,-3+1.57f*std::sin(a)},{.045f,1.35f,.045f},{.13f,.17f,.18f});}
    Render(cone,{0,9.f,-3},{2.2f,.85f,2.2f},{.26f,.14f,.11f});
    // Rocky coastline, dock, entrance trims and warm fixtures.
    for(int i=0;i<24;++i){float a=float(i)*6.2831853f/24;Render(cylinder,{12.9f*std::cos(a),-.2f,20.8f*std::sin(a)},{1.1f,.65f+(i%3)*.15f,1.3f},{.49f,.52f,.50f},2);}
    for(int i=0;i<8;++i)Render(cube,{0,.07f,17.4f+i*.4f},{2.3f,.14f,.35f},{.32f,.22f,.13f},3);
    for(float x:{-1.2f,1.2f})for(float z:{17.4f,20.2f})Render(cylinder,{x,.35f,z},{.12f,1.4f,.12f},{.24f,.17f,.11f},3);
    for(float x:{-1.08f,1.08f})Render(cube,{x,1.3f,5.18f},{.13f,2.6f,.20f},{.25f,.16f,.10f},3);
    for(float z:{2.f,-3.f}){
        Render(cylinder,{0,2.95f,z},{.12f,.35f,.12f},{.95f,.72f,.35f},6);
        Render(cone,{0,3.1f,z},{.4f,.18f,.4f},{.18f,.21f,.2f});
    }
    if(!investigation.DoorOpen())Render(cube,V(door.center),V(door.size),{.35f,.24f,.13f},3);
    // Boots by the entrance, matching the diary's owner, and a cordoned stair fragment.
    for(float x:{-1.8f,-1.5f}){Render(cube,{x,.18f,4.45f},{.20f,.35f,.30f},{.12f,.09f,.07f});Render(cube,{x,.10f,4.23f},{.20f,.15f,.28f},{.12f,.09f,.07f});}
    for(int i=0;i<4;++i)Render(cube,{-4.3f,.1f+i*.18f,1.5f-i*.30f},{.8f,.20f+i*.36f,.28f},{.32f,.34f,.33f},2);
    Render(cube,{-4.3f,1.1f,1.85f},{1.15f,.13f,.06f},{.86f,.58f,.15f});
    for(std::size_t i=0;i<cluePositions.size();++i){
        auto p=cluePositions[i];auto accent=(int(i)==target && investigation.screen==Screen::Explore)?glm::vec3(1,.83f,.33f):glm::vec3(.68f,.49f,.24f);
        if(i==0){Render(cube,p,{.6f,.13f,.44f},{.19f,.11f,.07f},3);Render(cube,p+glm::vec3(0,.075f,0),{.54f,.02f,.38f},{.88f,.82f,.66f});}
        if(i==1){for(float x:{-.10f,.10f}){Render(cube,p+glm::vec3(x,0,0),{.06f,.06f,.42f},{.45f,.18f,.12f});Render(cube,p+glm::vec3(x,.02f,-.24f),{.065f,.08f,.15f},accent);}Render(cube,p+glm::vec3(.27f,0,-.15f),{.07f,.07f,.08f},{.48f,.49f,.46f});}
        if(i==2){for(int j=0;j<5;++j){glm::vec3 q=p+glm::vec3(j%2?.20f:-.20f,0,(j-2)*.34f);Render(cube,q,{.17f,.014f,.27f},{.09f,.12f,.12f});Render(cube,q+glm::vec3(0,0,.19f),{.14f,.014f,.10f},{.09f,.12f,.12f});}}
        if(i==3){Render(cylinder,p,{.08f,.035f,.08f},accent);Render(cube,p+glm::vec3(.15f,0,0),{.24f,.035f,.04f},accent);Render(cube,p+glm::vec3(.22f,0,.035f),{.04f,.035f,.10f},accent);}
        if(i==4){Render(cube,p+glm::vec3(-.15f,0,0),{.26f,.015f,.36f},{.81f,.76f,.64f});Render(cube,p+glm::vec3(.16f,0,.03f),{.26f,.015f,.36f},{.81f,.76f,.64f});Render(cube,p+glm::vec3(0,.012f,0),{.34f,.01f,.10f},{.19f,.27f,.3f});}
    }
    shader.Use("SkyBox");shader.SetMat4("view",glm::mat4(glm::mat3(camera.GetViewMatrix())));shader.SetMat4("projection",projection);shader.SetInt("skyboxTextures",0);skybox->DrawSkybox();glDisable(GL_CULL_FACE);
    // Transparent surfaces after opaque geometry and sky. Farther water, then glass.
    shader.Use("Prototype");glActiveTexture(GL_TEXTURE3);glBindTexture(GL_TEXTURE_CUBE_MAP,skybox->GetSkyboxTexture());
    glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);glDepthMask(GL_FALSE);
    Render(cube,{0,-.55f,0},{180,.10f,180},{.06f,.17f,.22f},4,.90f);
    Render(cylinder,{0,8.f,-3},{1.55f,1.25f,1.55f},{.80f,.93f,1.f},5,.28f);
    glDepthMask(GL_TRUE);glDisable(GL_BLEND);glActiveTexture(GL_TEXTURE0);
}
