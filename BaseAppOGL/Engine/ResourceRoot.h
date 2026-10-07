#pragma once
#include <filesystem>
#include <iostream>
#include <windows.h>
inline bool PrepareResourceRoot(){
    wchar_t buffer[32768]{};auto count=GetModuleFileNameW(nullptr,buffer,32768);
    if(!count || count>=32768)return false;
    auto base=std::filesystem::path(buffer).parent_path();
    for(int depth=0;depth<4;++depth,base=base.parent_path()){
        if(std::filesystem::exists(base/L"Scenes/UltimoSinal/Prototype.vert")){
            std::filesystem::current_path(base);
            const char* required[]={"Fonts/arial.ttf","Scenes/Assets/Castle/Castle.obj","Scenes/Scene6/Statue/Statue.obj","Scenes/UltimoSinal/Prototype.vert","Scenes/UltimoSinal/Prototype.frag","Scenes/UltimoSinal/Sky.frag","Scenes/Common/SkyBox.vert","Scenes/Common/SkyBox.frag","Scenes/Skybox/right.bmp","Scenes/Skybox/left.bmp","Scenes/Skybox/top.bmp","Scenes/Skybox/bottom.bmp","Scenes/Skybox/front.bmp","Scenes/Skybox/back.bmp","Scenes/Textures/grass.bmp","Scenes/Textures/rock.jpg","Scenes/Textures/rockNormal.jpg"};
            for(auto file:required)if(!std::filesystem::exists(file)){std::cerr<<"Recurso ausente: "<<file<<std::endl;return false;}
            return true;
        }
    }
    std::cerr<<"Pasta Scenes nao encontrada junto ao executavel. Extraia o pacote completo, incluindo Scenes e Fonts."<<std::endl;return false;
}
