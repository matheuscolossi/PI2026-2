#include "UltimoSinal.h"
#include "Storage.h"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <windows.h>
namespace {
std::filesystem::path UserFolder(){wchar_t test[32768]{};auto t=GetEnvironmentVariableW(L"ULTIMO_TEST_HOME",test,32768);if(t&&t<32768)return std::filesystem::path(test);wchar_t location[32768]{};auto n=GetEnvironmentVariableW(L"LOCALAPPDATA",location,32768);if(n&&n<32768)return std::filesystem::path(location)/L"UltimoSinal";return std::filesystem::temp_directory_path()/L"UltimoSinal";}
bool Atomic(const std::filesystem::path& path,const std::string& text){
 std::error_code error;std::filesystem::create_directories(path.parent_path(),error);if(error)return false;
 auto temp=path;temp+=L".tmp";{std::ofstream out(temp,std::ios::binary|std::ios::trunc);out<<text;out.flush();if(!out)return false;}
 return MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
}
}
void UltimoSinal::Save(){
 if(experience.menuCameraHeld||experience.introPending||benchmarkMode||!sessionActive||investigation.screen==ultimo::Screen::Title||investigation.screen==ultimo::Screen::ConfirmNew)return;
 try{ultimo::Snapshot state;state.game=investigation;state.x=camera.Position.x;state.y=camera.Position.y;state.z=camera.Position.z;state.yaw=camera.Yaw;state.pitch=camera.Pitch;state.elapsed=duration;state.complete=investigation.submitted;
 auto payload=ultimo::Encode(state);ultimo::Snapshot validated;if(!ultimo::Decode(payload,validated)){saveNotice="O estado atual não pôde ser salvo; o arquivo anterior foi preservado.";return;}if(Atomic(UserFolder()/L"progress.v3","ULTIMO3 "+std::to_string(ultimo::Fingerprint(payload))+"\n"+payload)){hasSave=true;saveNotice="Progresso salvo neste computador.";}else saveNotice="Não foi possível salvar. O arquivo anterior foi preservado.";
 }catch(...){saveNotice="Pasta de salvamento indisponível; a sessão continua em memória.";}
}
bool UltimoSinal::Load(){
 try{ultimo::Snapshot state;if(!ultimo::ReadSave(UserFolder()/L"progress.v3",state)){saveNotice="O progresso não pôde ser lido ou pertence a outra versão. Ele foi preservado.";return false;}
 investigation=state.game;sessionActive=true;duration=state.elapsed;camera=CCamera({state.x,state.y,state.z},{0,1,0},state.yaw,state.pitch);camera.Zoom=58;
 inspectedEvidence=previewEvidence=-1;lensPlaybackTime=-1;answer.fill(0);std::copy(investigation.justification.begin(),investigation.justification.end(),answer.begin());target=-1;skipMouse=true;doorAmount=investigation.DoorOpen()?1.f:0.f;
 if(Blocked(camera.Position)||!ultimo::spatial::ValidPose(camera.Position.x,camera.Position.y,camera.Position.z)){camera.Position={0,1.7f,4};saveNotice="Progresso recuperado; posição ajustada para passagem segura.";}else saveNotice="Progresso recuperado.";
 ResetExperience(true);SyncCursor();return true;
 }catch(...){saveNotice="Falha ao ler o progresso; tente iniciar uma nova investigação.";return false;}
}
void UltimoSinal::DeleteSave(){try{std::error_code e;std::filesystem::remove(UserFolder()/L"progress.v3",e);if(!e)hasSave=false;}catch(...){} }
void UltimoSinal::LoadSettings(){
 try{auto folder=UserFolder();hasSave=std::filesystem::exists(folder/L"progress.v3");std::ifstream in(folder/L"options.v1");std::string magic;ultimo::Settings v;
 if(in>>magic>>v.sensitivity>>v.brightness>>v.master>>v.ambient>>v.effects>>v.invertY>>v.reduceMotion>>v.quality>>v.forward>>v.back>>v.left>>v.right>>v.interact>>v.journal){
  if(magic!="ULTIMO_OPTIONS1"||!std::isfinite(v.sensitivity)||!std::isfinite(v.brightness)||!std::isfinite(v.master)||!std::isfinite(v.ambient)||!std::isfinite(v.effects))return;
  v.sensitivity=v.Sensitivity();v.brightness=(std::clamp)(v.brightness,.75f,1.35f);v.master=(std::clamp)(v.master,0.f,1.f);v.ambient=(std::clamp)(v.ambient,0.f,1.f);v.effects=(std::clamp)(v.effects,0.f,1.f);v.quality=(std::clamp)(v.quality,0,2);
  int keys[]{v.forward,v.back,v.left,v.right,v.interact,v.journal};for(int i=0;i<6;++i){if(keys[i]<'A'||keys[i]>'Z')return;for(int j=0;j<i;++j)if(keys[j]==keys[i])return;}settings=v;
 }
 }catch(...){saveNotice="Opções padrão: pasta do usuário indisponível.";}
}
void UltimoSinal::SaveSettings(){
 try{std::ostringstream out;out<<"ULTIMO_OPTIONS1 "<<settings.Sensitivity()<<' '<<settings.brightness<<' '<<settings.master<<' '<<settings.ambient<<' '<<settings.effects<<' '<<settings.invertY<<' '<<settings.reduceMotion<<' '<<settings.quality<<' '<<settings.forward<<' '<<settings.back<<' '<<settings.left<<' '<<settings.right<<' '<<settings.interact<<' '<<settings.journal;
 if(!Atomic(UserFolder()/L"options.v1",out.str()))saveNotice="Não foi possível gravar as opções.";
 }catch(...){saveNotice="Não foi possível gravar as opções.";}
}
