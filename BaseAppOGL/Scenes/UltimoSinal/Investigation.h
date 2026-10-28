#pragma once
#include "Narrative.h"
#include <algorithm>
#include <string>
#include <cctype>
namespace ultimo {
enum class Screen{Title,Explore,Reading,Journal,Pause,Conclusion,Complete,Panel,Radio,Help,Options,ConfirmNew,Controls,Intro,LensConsole};
class Investigation{
public:
 Screen screen=Screen::Title,readingReturn=Screen::Explore,pauseReturn=Screen::Explore;
 ClueId reading=ClueId::Diary;int readingEntry=0,hypothesis=-1,radioChannel=16,helpStage=0;
 std::array<bool,EntryCount> discovered{},evidence{};
 std::array<bool,3> breakers{{true,false,false}};
 bool powerSolved=false,radioSolved=false,submitted=false;
 std::array<bool,3> lensLong{{false,false,false}};
 bool lensSolved=false,lensManual=false;
 std::string justification,notice;
 void Start(){*this=Investigation{};screen=Screen::Explore;}
 std::size_t Count()const{return std::count(discovered.begin(),discovered.begin()+5,true);}
 bool Found(ClueId id)const{auto i=std::size_t(id);return i<5&&discovered[i];}
 bool AllFound()const{return Count()==5;}
 bool DoorOpen()const{return Found(ClueId::Key);}
 bool Ready()const{return AllFound()&&radioSolved&&discovered[8]&&lensSolved;}
 bool InspectEntry(int id){
  if(screen!=Screen::Explore||id<0||id>=int(EntryCount))return false;
  if(id==3&&!powerSolved){notice="O armário de segurança permanece sem alimentação auxiliar.";return false;}
  if((id==4||id==8)&&!DoorOpen()){notice="A sala da lente está trancada.";return false;}
  if(id==7&&!radioSolved)return false;
  discovered[id]=true;readingEntry=id;if(id<5)reading=ClueId(id);readingReturn=Screen::Explore;screen=Screen::Reading;notice.clear();return true;
 }
 bool Inspect(ClueId id){return std::size_t(id)<ClueCount&&InspectEntry(int(id));}
 bool ReadEntry(int id){if(screen!=Screen::Journal||id<0||id>=int(EntryCount)||!discovered[id])return false;readingEntry=id;readingReturn=Screen::Journal;screen=Screen::Reading;return true;}
 bool ReadAgain(ClueId id){return ReadEntry(int(id));}
 void CloseReading(){if(screen==Screen::Reading)screen=readingReturn;}
 void ToggleJournal(){if(screen==Screen::Explore)screen=Screen::Journal;else if(screen==Screen::Journal)screen=Screen::Explore;}
 void TogglePause(){if(screen==Screen::Pause)screen=pauseReturn;else if(screen!=Screen::Title&&screen!=Screen::Complete&&screen!=Screen::ConfirmNew&&screen!=Screen::Options){pauseReturn=screen;screen=Screen::Pause;}}
 bool RestorePower(){
  if(screen!=Screen::Panel||!discovered[5]||!discovered[2]){notice="Faltam dados: consulte a manutenção e examine o acesso ao cais.";return false;}
  if(breakers[0]||!breakers[1]||!breakers[2]){notice="A proteção recusou a configuração. Cruze os ramais com os sinais de água salgada.";return false;}
  powerSolved=true;helpStage=0;notice="Rádio e serviço alimentados. A trava do armário respondeu.";return true;
 }
 bool TuneRadio(){
  if(screen!=Screen::Radio||!powerSolved){notice="O receptor está sem alimentação.";return false;}
  if(!discovered[6]){notice="Procure na carta de navegação o canal de registros locais.";return false;}
  if(radioChannel!=6){notice="Neste canal há apenas chamada ou tráfego distante. Consulte a carta.";return false;}
  radioSolved=true;helpStage=0;discovered[7]=true;readingEntry=7;readingReturn=Screen::Radio;screen=Screen::Reading;notice="Transmissão arquivada no caderno.";return true;
 }
 // Local diagnostic only: compare the selected reconstruction with existing records.
 // It neither operates the lighthouse transmitter nor adds a historical observation.
 bool VerifyLens(){
  if(screen!=Screen::LensConsole){notice="Use o console de diagnóstico na sala da lente.";return false;}
  if(submitted||lensSolved){notice="A reconstrução já foi conferida e está preservada no caderno.";return false;}
  if(!powerSolved||!DoorOpen()){notice="O console depende da alimentação de serviço e do acesso à sala da lente.";return false;}
  if(!radioSolved||!discovered[4]||!discovered[8]){notice="Reúna a transmissão de Brumas, a fotografia e o registro mecânico antes de conferir a sequência.";return false;}
  if(!lensManual||!std::all_of(lensLong.begin(),lensLong.end(),[](bool v){return v;})){
   notice="A configuração não explica os registros. Compare o modo do mecanismo e a duração dos três pulsos.";return false;
  }
  lensSolved=true;helpStage=0;notice="Reconstrução local conferida: modo manual e três pulsos longos. Nenhum sinal foi transmitido.";return true;
 }
 bool BeginConclusion(){if(submitted){notice="O relatório já foi transmitido. Seus registros continuam disponíveis para consulta.";return false;}if(screen!=Screen::Explore||!Ready()){notice="Seu relatório ainda precisa explicar o horário, o modo de operação e o significado dos pulsos.";return false;}screen=Screen::Conclusion;return true;}
 bool CanSubmit()const{return !submitted&&screen==Screen::Conclusion&&Ready()&&hypothesis>=0&&hypothesis<3&&std::count(evidence.begin(),evidence.end(),true)>=2&&std::any_of(justification.begin(),justification.end(),[](unsigned char c){return !std::isspace(c);});}
 bool Submit(){if(!CanSubmit())return false;submitted=true;screen=Screen::Complete;return true;}
 bool Correct()const{return screen==Screen::Complete&&hypothesis==1;}
 const char* Question()const{
  if(!powerSolved)return "Por que o farol ainda transmitiu depois da chamada de emergência?";
  if(!radioSolved)return "Que horário Brumas registrou — e em qual canal?";
  if(!DoorOpen())return "O que a alimentação de serviço liberou dentro do armário?";
  if(!discovered[8])return "O temporizador avariado podia produzir o sinal?";
  if(!discovered[4])return "A fotografia revela o significado da sequência registrada?";
  if(!lensSolved)return "Qual configuração do console explica o último sinal?";
  return "Para quem eram os três pulsos longos?";
 }
 const char* Hint()const{
  if(!powerSolved){if(helpStage<2)return "A manutenção descreve um procedimento. O lado de fora mostra a condição real do equipamento.";if(helpStage==2)return "Compare a cor do ramal molhado com a ficha; rádio e serviço precisam de energia.";return "Examine as marcas e a ficha. No painel: CAIS desligado, RÁDIO e SERVIÇO ligados; depois rearme.";}
  if(!radioSolved){if(helpStage<2)return "A estação local não arquiva informações no canal de emergência.";if(helpStage==2)return "A carta de navegação identifica a estação e seu canal.";return "Leia a carta perto da entrada. Ligue o rádio no canal 6 e escute; a transcrição entra no caderno.";}
  if(!DoorOpen())return "O serviço reativado libera a trava do armário junto à parede. Examine seu interior.";
  if(!lensSolved){
   if(helpStage<2)return "Na sala da lente, compare o registro mecânico, a transmissão e a fotografia. O console permite testar uma reconstrução local.";
   if(helpStage==2)return "O temporizador estava avariado. O registro mostra outro modo de operação, e o verso da fotografia descreve a duração dos pulsos.";
   return "Leia o registro e a fotografia. No console da lente, selecione MANUAL e três pulsos LONGOS; depois confira a reconstrução. É um teste local, sem transmissão.";
  }
  return "Relacione: queda às 23h10; comando manual às 23h17; significado dos pulsos na fotografia. Complete as evidências e apresente seu relatório.";
 }
};
}
