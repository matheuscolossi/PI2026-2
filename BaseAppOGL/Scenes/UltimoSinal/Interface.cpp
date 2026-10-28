#include "UltimoSinal.h"
#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <string>

using ultimo::Screen;
namespace {
const ImVec4 paper(.89f,.87f,.80f,1.f),muted(.56f,.62f,.63f,1.f),gold(.84f,.68f,.39f,1.f);
float Unit(float v){return (std::clamp)(v,0.f,1.f);}
ImU32 Ink(int r,int g,int b,float a=1.f){return IM_COL32(r,g,b,int(Unit(a)*255));}
void Letter(ImDrawList* draw,ImVec2 p,float size,ImU32 color,const char* text,float wrap=0.f){
    draw->AddText(ImGui::GetFont(),size,p,color,text,nullptr,wrap);
}
void CoastMark(ImDrawList* draw,ImVec2 p,float scale,float opacity=1.f){
    // A restrained nautical device, drawn with the same type/line palette as the dossier.
    const auto c=Ink(212,176,111,.62f*opacity);
    draw->AddCircle(p,19*scale,c,48,1.f);draw->AddCircle(p,13*scale,Ink(212,176,111,.22f*opacity),40,1.f);
    draw->AddLine(ImVec2(p.x-25*scale,p.y),ImVec2(p.x+25*scale,p.y),c,1.f);
    draw->AddLine(ImVec2(p.x,p.y-25*scale),ImVec2(p.x,p.y+25*scale),c,1.f);
    draw->AddTriangleFilled(ImVec2(p.x,p.y-13*scale),ImVec2(p.x-4*scale,p.y+7*scale),ImVec2(p.x+4*scale,p.y+7*scale),Ink(212,176,111,.75f*opacity));
}
bool MenuAction(const char* label,const char* detail,bool primary,float height=52.f){
    ImGui::PushID(label);const ImVec2 p=ImGui::GetCursorScreenPos();const float w=ImGui::GetContentRegionAvail().x;
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(0,0,0,0));ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(0,0,0,0));ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(0,0,0,0));
    const bool clicked=ImGui::Button("##menu",ImVec2(w,height));ImGui::PopStyleColor(3);
    if(primary)ImGui::SetItemDefaultFocus();
    auto* d=ImGui::GetWindowDrawList();const bool hover=ImGui::IsItemHovered()||ImGui::IsItemFocused();
    const float disabled=ImGui::GetStyle().Alpha;
    d->AddRectFilled(p,ImVec2(p.x+w,p.y+height),primary?Ink(190,150,81,(hover?.29f:.15f)*disabled):Ink(175,200,205,(hover?.09f:.025f)*disabled),2.f);
    d->AddRect(p,ImVec2(p.x+w,p.y+height),primary?Ink(212,176,111,(hover?.85f:.47f)*disabled):Ink(170,197,200,(hover?.35f:.08f)*disabled),2.f);
    if(primary||hover)d->AddRectFilled(p,ImVec2(p.x+2.f,p.y+height),Ink(226,191,129,disabled));
    Letter(d,ImVec2(p.x+18,p.y+9),18,Ink(235,231,215,disabled),label);
    Letter(d,ImVec2(p.x+18,p.y+31),10.5f,Ink(153,170,171,.90f*disabled),detail);
    Letter(d,ImVec2(p.x+w-24,p.y+15),18,Ink(212,176,111,(hover?1.f:.55f)*disabled),">");
    ImGui::PopID();return clicked;
}
std::string KeyLabel(int key) {
    if(key==GLFW_KEY_SPACE)return "Espaço";
    if(key==GLFW_KEY_ENTER)return "Enter";
    if(key==GLFW_KEY_TAB)return "Tab";
    if(key==GLFW_KEY_ESCAPE)return "Esc";
    const char* name=glfwGetKeyName(key,0);
    if(name){std::string s(name);if(s.size()==1&&s[0]>='a'&&s[0]<='z')s[0]-=('a'-'A');return s;}
    return std::string("Tecla ")+std::to_string(key);
}
void Heading(const char* eyebrow,const char* title) {
    ImGui::TextColored(gold,"%s",eyebrow);ImGui::Spacing();
    ImGui::SetWindowFontScale(1.35f);ImGui::TextWrapped("%s",title);ImGui::SetWindowFontScale(1.f);
    ImGui::Spacing();ImGui::Separator();ImGui::Spacing();
}
void Paragraph(const char* text){ImGui::TextWrapped("%s",text);ImGui::Spacing();}
bool Action(const char* label,bool primary=false) {
    if(primary)ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.31f,.27f,.18f,1));
    bool clicked=ImGui::Button(label,ImVec2(-1,38));
    if(primary)ImGui::PopStyleColor();return clicked;
}
const char* EntryKind(int id) {
    if(id==5)return "MANUTENÇÃO";if(id==6)return "NAVEGAÇÃO";if(id==7)return "TRANSCRIÇÃO";
    if(id==8)return "MECANISMO";if(id==9)return "MEMÓRIA PESSOAL";return "EVIDÊNCIA";
}
}

void UltimoSinal::DrawInterface() {
    const ImVec2 display=ImGui::GetIO().DisplaySize;
    if(display.x<=0||display.y<=0)return;
    const auto fixed=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar;
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.055f,.072f,.085f,.97f));
    ImGui::PushStyleColor(ImGuiCol_Text,paper);ImGui::PushStyleColor(ImGuiCol_TextDisabled,muted);
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.14f,.19f,.20f,1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(.23f,.29f,.29f,1));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,ImVec4(.33f,.33f,.25f,1));
    ImGui::PushStyleColor(ImGuiCol_CheckMark,gold);ImGui::PushStyleColor(ImGuiCol_SliderGrab,gold);
    ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(.11f,.15f,.17f,1));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,ImVec4(.18f,.24f,.25f,1));
    ImGui::PushStyleColor(ImGuiCol_Header,ImVec4(.19f,.25f,.25f,1));
    ImGui::PushStyleColor(ImGuiCol_NavCursor,gold);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(26,23));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(10,10));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(9,7));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,4.f);ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,3.f);
    const auto openMenu=[&](Screen next){menuReturn=int(investigation.screen);investigation.screen=next;};
    const auto returnMenu=[&](){investigation.screen=Screen(menuReturn);};
    const auto newGame=[&](){if(hasSave||sessionActive)openMenu(Screen::ConfirmNew);else Start();};
    const auto quit=[&](){Save();glfwSetWindowShouldClose(glfwGetCurrentContext(),GL_TRUE);};

    if(investigation.screen==Screen::Title) {
        auto* d=ImGui::GetBackgroundDrawList();
        const float safe=(std::clamp)(display.x*.052f,24.f,76.f);
        const bool compact=display.x<800.f||display.y<570.f;
        const float titleSize=(std::clamp)(display.x*.047f,32.f,76.f);
        const float titleY=compact?display.y*.12f:display.y*.16f;
        const float menuY=titleY+titleSize*2.23f+53.f;
        const float menuWidth=display.x<680.f?display.x-safe*2:(std::min)(380.f,display.x*.40f);
        // Leave the right-hand skyline visible. The menu is a layer over the actual 3D scene.
        d->AddRectFilledMultiColor(ImVec2(0,0),display,Ink(4,11,15,.98f),Ink(4,11,15,.06f),Ink(4,11,15,.27f),Ink(4,11,15,.98f));
        d->AddRectFilledMultiColor(ImVec2(0,display.y*.68f),display,Ink(3,8,11,0),Ink(3,8,11,0),Ink(3,8,11,.92f),Ink(3,8,11,.92f));
        d->AddLine(ImVec2(safe,39),ImVec2(display.x-safe,39),Ink(172,195,198,.18f));
        Letter(d,ImVec2(safe,20),11.f,Ink(199,173,126),"BRUMAS  /  ARQUIVO DE CAMPO 01");
        if(display.x>760.f)Letter(d,ImVec2(display.x-safe-253,20),10.5f,Ink(151,172,177),"MISTÉRIO EM PRIMEIRA PESSOA");
        Letter(d,ImVec2(safe,titleY-23),11.f,Ink(203,174,120),"UMA INVESTIGAÇÃO NA COSTA DO SILÊNCIO");
        Letter(d,ImVec2(safe+2,titleY+2),titleSize,Ink(0,0,0,.65f),"O ÚLTIMO");
        Letter(d,ImVec2(safe,titleY),titleSize,Ink(238,234,217),"O ÚLTIMO");
        Letter(d,ImVec2(safe,titleY+titleSize*.98f),titleSize,Ink(215,183,123),"SINAL");
        d->AddLine(ImVec2(safe,titleY+titleSize*2.06f),ImVec2(safe+48,titleY+titleSize*2.06f),Ink(215,183,123,.8f),2.f);
        Letter(d,ImVec2(safe,titleY+titleSize*2.23f),14.f,Ink(177,192,192),"A luz acendeu depois da notícia da morte.",menuWidth);

        ImGui::SetNextWindowPos(ImVec2(safe-8,menuY),ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(menuWidth+16,(std::max)(110.f,display.y-menuY-43.f)));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(8,4));ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,7));
        ImGui::Begin("##title-actions",nullptr,fixed|ImGuiWindowFlags_NoBackground);
        const float actionHeight=compact?46.f:52.f;
        ImGui::BeginDisabled(!hasSave&&!sessionActive);
        if(MenuAction("Continuar investigação",sessionActive?"RETOMAR ESTA SESSÃO":hasSave?"RETOMAR O PROGRESSO SALVO":"NENHUM PROGRESSO LOCAL",hasSave||sessionActive,actionHeight)){
            if(sessionActive){if(experience.introPending)FinishIntro();else{RestorePlayerCamera();investigation.screen=investigation.submitted?Screen::Complete:Screen::Explore;}}else Load();
        }ImGui::EndDisabled();
        if(MenuAction("Nova investigação","CHEGAR À ILHA E ABRIR O CASO",!hasSave&&!sessionActive,actionHeight))newGame();
        if(MenuAction("Opções","IMAGEM, CÂMERA E SOM",false,actionHeight))openMenu(Screen::Options);
        if(MenuAction("Controles","MOVIMENTO E INTERAÇÃO",false,actionHeight))openMenu(Screen::Controls);
        if(MenuAction("Sair","ENCERRAR O JOGO",false,actionHeight))quit();
        if(!saveNotice.empty()){ImGui::Spacing();ImGui::TextWrapped("%s",saveNotice.c_str());}
        ImGui::End();ImGui::PopStyleVar(2);

        if(display.x>=1060.f&&display.y>=620.f){
            const float cardW=(std::min)(300.f,display.x*.23f),cardX=display.x-safe-cardW,cardY=display.y*.56f;
            d->AddRectFilled(ImVec2(cardX,cardY),ImVec2(cardX+cardW,cardY+196),Ink(7,17,22,.78f),3.f);
            d->AddRect(ImVec2(cardX,cardY),ImVec2(cardX+cardW,cardY+196),Ink(178,194,188,.17f),3.f);
            CoastMark(d,ImVec2(cardX+35,cardY+38),.65f);
            Letter(d,ImVec2(cardX+65,cardY+25),11.f,Ink(210,180,124),"DOSSIÊ DA ESTAÇÃO");
            Letter(d,ImVec2(cardX+65,cardY+43),11.f,Ink(157,176,180),"ILHA DAS BRUMAS / 05H40");
            d->AddLine(ImVec2(cardX+20,cardY+72),ImVec2(cardX+cardW-20,cardY+72),Ink(178,194,188,.14f));
            Letter(d,ImVec2(cardX+20,cardY+88),15.f,Ink(220,224,216),"Uma estação isolada.\nUm último sinal.\nUma versão que não fecha.",cardW-40);
            Letter(d,ImVec2(cardX+20,cardY+158),11.f,Ink(152,174,178),"EXPLORE  /  ESCUTE  /  RELACIONE");
        }
        const float markTime=settings.reduceMotion?0.f:experience.menuTime;
        const float pulse=settings.reduceMotion?.7f:.55f+.2f*std::sin(markTime*.8f);
        d->AddCircleFilled(ImVec2(safe+3,display.y-22),2.5f,Ink(211,180,118,pulse));
        Letter(d,ImVec2(safe+15,display.y-28),10.5f,Ink(144,166,170),"JOGADOR ÚNICO  /  PROGRESSO LOCAL");
        if(display.x>830.f)Letter(d,ImVec2(display.x-safe-231,display.y-28),10.5f,Ink(130,153,159),"OBSERVE. ESCUTE. RELACIONE.");
    } else if(investigation.screen==Screen::Intro) {
        auto* d=ImGui::GetBackgroundDrawList();const float t=experience.introTime;
        const float safe=(std::clamp)(display.x*.072f,24.f,100.f),bar=(std::clamp)(display.y*.105f,30.f,86.f);
        const float fade=settings.reduceMotion?1.f:Unit(t/1.2f);
        d->AddRectFilled(ImVec2(0,0),display,Ink(3,9,13,.40f));
        d->AddRectFilledMultiColor(ImVec2(0,display.y*.4f),display,Ink(3,8,12,0),Ink(3,8,12,0),Ink(3,8,12,.94f),Ink(3,8,12,.94f));
        d->AddRectFilled(ImVec2(0,0),ImVec2(display.x,bar),Ink(0,0,0,.97f));
        d->AddRectFilled(ImVec2(0,display.y-bar),display,Ink(0,0,0,.97f));
        Letter(d,ImVec2(safe,bar+28),11.f,Ink(215,184,126,fade),"PRÓLOGO  /  A CHEGADA");
        const float size=(std::clamp)(display.x*.032f,27.f,46.f);
        Letter(d,ImVec2(safe,bar+55),size,Ink(235,233,220,fade),"Ilha das Brumas");
        Letter(d,ImVec2(safe,bar+61+size),14.f,Ink(172,194,199,fade),"05h40  /  Costa sul");
        const int phase=t<3.2f?0:t<7.2f?1:2;
        const char* lead=phase==0?"O rádio voltou a ficar em silêncio.":phase==1?"Augusto, o faroleiro, foi encontrado morto.":"Entre na estação. Descubra o que ficou para trás.";
        const char* supporting=phase==0?"Na madrugada, a estação enviou um sinal que ninguém consegue explicar.":phase==1?"Há horários que não coincidem. Sua tarefa é reconstruir a noite, a partir dos vestígios.":"Observe os objetos, leia os registros e confronte as versões. O caderno preserva cada descoberta.";
        const float phaseStart=phase==0?0.f:phase==1?3.2f:7.2f;
        const float textFade=settings.reduceMotion?1.f:Unit((t-phaseStart)/.65f);
        const float bodyY=(std::max)(bar+size+120.f,display.y-bar-168.f);
        const float leadSize=display.x<750.f?20.f:25.f;
        const auto leadBounds=ImGui::GetFont()->CalcTextSizeA(leadSize,1e30f,display.x-safe*2,lead);
        Letter(d,ImVec2(safe,bodyY),leadSize,Ink(235,232,216,textFade),lead,display.x-safe*2);
        Letter(d,ImVec2(safe,bodyY+leadBounds.y+14.f),15.f,Ink(170,190,195,textFade),supporting,(std::min)(750.f,display.x-safe*2));
        const float usable=display.x-safe*2;
        d->AddRectFilled(ImVec2(safe,display.y-bar-9),ImVec2(safe+usable,display.y-bar-7),Ink(178,194,188,.12f));
        d->AddRectFilled(ImVec2(safe,display.y-bar-9),ImVec2(safe+usable*Unit(t/ultimo::IntroDuration),display.y-bar-7),Ink(212,178,115,.65f));
        ImGui::SetNextWindowPos(ImVec2(display.x-safe,display.y-bar*.5f),ImGuiCond_Always,ImVec2(1,.5f));
        ImGui::SetNextWindowSize(ImVec2((std::min)(330.f,display.x-safe*2),0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
        ImGui::Begin("##intro-controls",nullptr,fixed|ImGuiWindowFlags_NoBackground|ImGuiWindowFlags_AlwaysAutoResize);
        if(ImGui::Button("Começar investigação  [Enter]",ImVec2(-1,36)))FinishIntro();ImGui::End();ImGui::PopStyleVar();
        if(display.x>1000.f){const std::string controls=KeyLabel(settings.forward)+KeyLabel(settings.left)+KeyLabel(settings.back)+KeyLabel(settings.right)+"  mover     Mouse  observar     "+KeyLabel(settings.interact)+"  examinar";
            Letter(d,ImVec2(safe,display.y-bar*.5f-7),12.f,Ink(151,174,178),controls.c_str());}
    } else if(investigation.screen==Screen::Complete) {
        auto* d=ImGui::GetBackgroundDrawList();d->AddRectFilled(ImVec2(0,0),display,Ink(4,11,16,.86f));
        const float width=(std::min)(900.f,display.x-32.f),height=(std::min)(760.f,display.y-32.f);
        ImGui::SetNextWindowPos(ImVec2(display.x*.5f,display.y*.5f),ImGuiCond_Always,ImVec2(.5f,.5f));ImGui::SetNextWindowSize(ImVec2(width,height));
        ImGui::Begin("##case-closed",nullptr,fixed);
        Heading("O ÚLTIMO SINAL / FIM DA INVESTIGAÇÃO",investigation.Correct()?"A luz tinha um destinatário":"Uma hipótese precisa de sustentação");
        ImGui::TextColored(gold,"RELATÓRIO TRANSMITIDO");ImGui::Spacing();Paragraph(ultimo::Explanation);
        if(ImGui::CollapsingHeader("Seu relatório antes do resultado")){
            if(investigation.hypothesis>=0&&investigation.hypothesis<int(ultimo::Hypotheses.size()))Paragraph(ultimo::Hypotheses[investigation.hypothesis]);
            for(int i=0;i<int(ultimo::EntryCount);++i)if(investigation.evidence[i]&&investigation.discovered[i])ImGui::BulletText("%s",ultimo::Entries[i].name);
            Paragraph(investigation.justification.c_str());
        }
        ImGui::Spacing();ImGui::Separator();ImGui::Spacing();
        if(investigation.discovered[9])Paragraph("Antes de partir, você guarda a carta que Augusto nunca enviou. A cadeira voltada para o mar já não parece um objeto esquecido: é o lugar que ele reservava para Helena.");
        else Paragraph("O relatório segue para Brumas. A sala fica silenciosa; o farol continua guardando a vida de quem esteve aqui.");
        ImGui::TextColored(gold,"UMA ÚLTIMA CHAMADA");
        Paragraph("Na faixa de chamada aparece uma última mensagem: ‘O farol já pode voltar a acender?’ A origem permanece desconhecida.");
        ImGui::Spacing();ImGui::TextDisabled("Tempo desta sessão: %.0f min. A qualidade da justificativa é avaliada por uma pessoa.",duration/60.);
        ImGui::Spacing();if(Action("Consultar registros"))investigation.screen=Screen::Journal;
        if(Action("Voltar ao início",true)){Save();investigation.screen=Screen::Title;}
        if(Action("Nova investigação"))newGame();
        if(!saveNotice.empty()){ImGui::Separator();ImGui::TextWrapped("%s",saveNotice.c_str());}ImGui::End();
    } else if(investigation.screen==Screen::Explore) {
        ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(display.x*.5f,display.y*.5f),1.5f,IM_COL32(238,224,187,180));
        ImGui::SetNextWindowPos(ImVec2(20,display.y-20),ImGuiCond_Always,ImVec2(0,1));ImGui::SetNextWindowBgAlpha(.48f);
        ImGui::Begin("##fieldnotes",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoInputs);
        ImGui::TextColored(muted,"%s  Caderno    Tab  Pausa",KeyLabel(settings.journal).c_str());ImGui::End();
        if(target>=0&&target<int(interactionPositions.size())) {
            const char* name=target==3?(investigation.powerSolved?"Chave da passagem":"Armário de segurança"):target<int(ultimo::EntryCount)?ultimo::Entries[target].name:target==10?"Quadro de emergência":target==11?"Receptor de rádio":"Comando da lente";
            ImGui::SetNextWindowPos(ImVec2(display.x*.5f,display.y*.81f),ImGuiCond_Always,ImVec2(.5f,0));ImGui::SetNextWindowBgAlpha(.85f);
            ImGui::Begin("##interaction",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoInputs);
            ImGui::TextColored(gold,"%s",KeyLabel(settings.interact).c_str());ImGui::SameLine();ImGui::TextUnformatted(name);ImGui::End();
        }
        if(toastTime>0&&!investigation.notice.empty()) {
            ImGui::SetNextWindowPos(ImVec2(display.x-20,(display.x<1020.f||display.y<550.f)?display.y*.33f:24.f),ImGuiCond_Always,ImVec2(1,0));
            ImGui::SetNextWindowSize(ImVec2((std::min)(390.f,display.x-40),0));
            ImGui::Begin("##notice",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoInputs);
            Paragraph(investigation.notice.c_str());ImGui::End();
        }
    } else {
        auto* backdrop=ImGui::GetBackgroundDrawList();
        backdrop->AddRectFilledMultiColor(ImVec2(0,0),display,Ink(4,10,14,.83f),Ink(4,10,14,.42f),Ink(4,10,14,.63f),Ink(4,10,14,.83f));
        const bool pause=investigation.screen==Screen::Pause;
        float width=(std::max)(190.f,(std::min)(pause?455.f:790.f,display.x-32));
        float height=(std::min)(700.f,(std::max)(200.f,display.y-36));
        ImGui::SetNextWindowPos(ImVec2(pause?display.x*.08f:display.x*.5f,display.y*.5f),ImGuiCond_Always,ImVec2(pause?0.f:.5f,.5f));
        ImGui::SetNextWindowSize(ImVec2(width,height));ImGui::Begin("##investigation",nullptr,fixed);
        switch(investigation.screen) {
        case Screen::Reading: {
            int id=(std::clamp)(investigation.readingEntry,0,int(ultimo::EntryCount)-1);
            Heading(EntryKind(id),ultimo::Entries[id].name);Paragraph(ultimo::Entries[id].description);
            if(id==0||id==1||id==3||id==4){
                ImGui::Spacing();ImGui::Separator();ImGui::Spacing();
                if(ImGui::Button(inspectedEvidence==id?"Fechar inspeção":"Ver objeto",ImVec2(-1,34)))inspectedEvidence=inspectedEvidence==id?-1:id;
                if(inspectedEvidence==id)DrawEvidencePreview(id);
            }
            if(id==8&&camera.Position.y<8.5f)Paragraph("O comando está na sala óptica, no alto da escada em espiral do arquivo.");
            if(id==8&&investigation.DoorOpen()&&camera.Position.y>8.5f){
                ImGui::Spacing();
                if(Action("Examinar comando da lente")){inspectedEvidence=-1;investigation.screen=Screen::LensConsole;}
            }
            ImGui::Spacing();ImGui::TextDisabled("Registro disponível para releitura no caderno.");
            std::string back=std::string("Guardar leitura  [")+KeyLabel(settings.interact)+"]";
            if(Action(back.c_str(),true)){inspectedEvidence=-1;investigation.CloseReading();}break;
        }
        case Screen::Journal: {
            Heading("CADERNO DE CAMPO","O que sabemos até aqui");ImGui::TextColored(gold,"PERGUNTA EM ABERTO");Paragraph(investigation.Question());
            if(investigation.lensSolved){ImGui::TextColored(gold,"RECONSTITUIÇÃO CONFERIDA");Paragraph("O teste local do comando da lente foi concluído. Use os registros reunidos para sustentar sua interpretação no relatório.");}
            else if(investigation.discovered[8])Paragraph("O registro mecânico está arquivado. A bancada da lente permite conferir uma reconstituição com os dados de outros registros.");
            else if(investigation.discovered[7])Paragraph("O relato de Brumas deixa um intervalo por explicar. Procure um registro independente do equipamento.");
            else Paragraph("O caderno guarda o que você examinou. Reabra os registros para comparar detalhes; as perguntas mudam conforme a investigação avança.");
            ImGui::Separator();ImGui::Spacing();
            if(investigation.discovered[7]||investigation.discovered[8]){
                if(ImGui::CollapsingHeader("Horários documentados",ImGuiTreeNodeFlags_DefaultOpen)){
                    if(investigation.discovered[8])ImGui::BulletText("22h40 — avaria anotada no registro mecânico.");
                    if(investigation.discovered[7])ImGui::BulletText("23h10 — chamada de emergência registrada por Brumas.");
                    ImGui::BulletText("23h17 — sinal anotado nos registros da estação.");
                    ImGui::TextWrapped("Um horário registrado não identifica, por si só, quem estava no local.");ImGui::Spacing();
                }
            }
            const auto entryGroup=[&](const char* title,const std::initializer_list<int>& ids){
                int count=0;for(int id:ids)if(investigation.discovered[id])++count;
                if(!count)return;
                const std::string label=std::string(title)+"  /  "+std::to_string(count);
                if(ImGui::CollapsingHeader(label.c_str(),ImGuiTreeNodeFlags_DefaultOpen)){
                    for(int id:ids)if(investigation.discovered[id]){
                        ImGui::PushID(id);ImGui::TextColored(muted,"%s",EntryKind(id));
                        if(Action(ultimo::Entries[id].name)){inspectedEvidence=-1;investigation.ReadEntry(id);}ImGui::PopID();
                    }
                }
            };
            entryGroup("Registros e comunicações",{0,5,6,7,8});
            entryGroup("Vestígios e objetos",{1,2,3,4});
            entryGroup("História pessoal",{9});
            if(std::none_of(investigation.discovered.begin(),investigation.discovered.end(),[](bool found){return found;}))Paragraph("Nenhum registro ainda. Examine documentos, instalações e marcas no ambiente; suas descobertas ficam reunidas aqui.");
            ImGui::Spacing();
            if(investigation.submitted&&Action("Consultar desfecho",true))investigation.screen=Screen::Complete;
            if(!investigation.submitted&&investigation.Ready()&&Action("Preparar relatório",true)){investigation.screen=Screen::Explore;investigation.BeginConclusion();}
            if(Action("Pedir uma orientação"))openMenu(Screen::Help);
            std::string close=std::string("Fechar caderno  [")+KeyLabel(settings.journal)+"]";
            if(Action(close.c_str()))investigation.screen=Screen::Explore;break;
        }
        case Screen::Pause:
            Heading("INVESTIGAÇÃO EM PAUSA","A ilha espera");
            if(Action("Retomar  [Tab]",true))investigation.TogglePause();
            if(Action("Salvar progresso"))Save();
            if(Action("Opções"))openMenu(Screen::Options);
            if(Action("Controles"))openMenu(Screen::Controls);
            if(Action("Ajuda gradual"))openMenu(Screen::Help);
            if(Action("Voltar ao início")){Save();investigation.screen=Screen::Title;}
            if(Action("Sair do jogo"))quit();
            ImGui::TextDisabled("O progresso também é salvo após descobertas e mecanismos.");break;
        case Screen::Panel: {
            Heading("INSTALAÇÃO ELÉTRICA","Quadro de emergência");
            Paragraph("Três ramais, uma proteção. Uma configuração inadequada será recusada.");
            if(investigation.powerSolved){ImGui::TextColored(gold,"PROTEÇÃO REARMADA");Paragraph("A alimentação auxiliar do rádio e do serviço está estável.");}
            else {
                const char* names[]{"AZUL / Cais","BRANCO / Rádio","ÂMBAR / Serviço"};
                const ImU32 wire[]{Ink(91,152,194),Ink(218,225,211),Ink(220,177,95)};
                for(int i=0;i<3;++i){
                    ImGui::PushID(i);const auto p=ImGui::GetCursorScreenPos();const float w=ImGui::GetContentRegionAvail().x;
                    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.075f,.10f,.115f,1));
                    if(ImGui::Button("##breaker",ImVec2(w,66))){investigation.breakers[i]=!investigation.breakers[i];audio.Cue(1);Save();}ImGui::PopStyleColor();
                    auto* d=ImGui::GetWindowDrawList();d->AddRectFilled(ImVec2(p.x+1,p.y+1),ImVec2(p.x+4,p.y+65),wire[i]);
                    Letter(d,ImVec2(p.x+19,p.y+13),17.f,Ink(227,230,218),names[i]);
                    Letter(d,ImVec2(p.x+19,p.y+39),11.f,Ink(145,167,174),"CLIQUE PARA ALTERNAR O DISJUNTOR");
                    const float ledX=p.x+w-103;
                    d->AddCircleFilled(ImVec2(ledX,p.y+31),3.5f,investigation.breakers[i]?Ink(218,185,115):Ink(84,102,108));
                    Letter(d,ImVec2(ledX+12,p.y+24),12.f,investigation.breakers[i]?Ink(224,194,132):Ink(139,160,167),investigation.breakers[i]?"LIGADO":"DESLIGADO");
                    ImGui::PopID();
                }
                ImGui::Spacing();if(Action("Rearmar proteção",true)){bool restored=investigation.RestorePower();audio.Cue(restored?1:4);Save();}
            }
            if(!investigation.notice.empty())Paragraph(investigation.notice.c_str());
            if(Action("Consultar caderno"))investigation.screen=Screen::Journal;
            if(Action("Preciso de uma orientação"))openMenu(Screen::Help);
            if(Action("Afastar-se do quadro"))investigation.screen=Screen::Explore;break;
        }
        case Screen::Radio:
            Heading("ESTAÇÃO DO FAROL","Receptor de rádio");
            Paragraph(investigation.powerSolved?"A alimentação auxiliar está estável. Selecione uma frequência e escute.":"O receptor está silencioso. Falta alimentação auxiliar.");
            {
                const auto p=ImGui::GetCursorScreenPos();const float w=ImGui::GetContentRegionAvail().x;auto* d=ImGui::GetWindowDrawList();
                ImGui::Dummy(ImVec2(w,105));d->AddRectFilled(p,ImVec2(p.x+w,p.y+105),Ink(4,15,16),4.f);d->AddRect(p,ImVec2(p.x+w,p.y+105),Ink(108,145,135,.38f),4.f);
                Letter(d,ImVec2(p.x+20,p.y+16),11.f,Ink(115,157,142),"RECEPTOR COSTEIRO / 16 CANAIS");
                const std::string channel=std::string("CH ")+(investigation.radioChannel<10?"0":"")+std::to_string(investigation.radioChannel);
                Letter(d,ImVec2(p.x+18,p.y+37),32.f,investigation.powerSolved?Ink(181,212,161):Ink(71,94,87),channel.c_str());
                Letter(d,ImVec2(p.x+20,p.y+80),10.f,Ink(127,163,148),investigation.discovered[7]?"REGISTRO LOCAL ARQUIVADO":investigation.powerSolved?"AGUARDANDO ESCUTA":"SEM ALIMENTAÇÃO");
                if(w>350.f){const float meterX=p.x+w-145;for(int i=0;i<16;++i){const float x=meterX+i*6.f;
                    d->AddLine(ImVec2(x,p.y+43),ImVec2(x,p.y+(i%4==0?64.f:56.f)),Ink(101,149,127,.55f));}
                    const float needle=meterX+(investigation.radioChannel-1)*6.f;d->AddLine(ImVec2(needle,p.y+37),ImVec2(needle,p.y+68),Ink(211,186,113),2.f);}
            }
            if(ImGui::SliderInt("Canal",&investigation.radioChannel,1,16))Save();
            ImGui::BeginDisabled(!investigation.powerSolved);
            if(Action("Escutar canal",true)){bool tuned=investigation.TuneRadio();audio.Cue(tuned?2:4);Save();}ImGui::EndDisabled();
            if(!investigation.notice.empty())Paragraph(investigation.notice.c_str());
            if(investigation.discovered[7]){ImGui::Separator();ImGui::TextColored(gold,"TRANSCRIÇÃO ARQUIVADA");Paragraph(ultimo::Entries[7].description);}
            if(Action("Consultar caderno"))investigation.screen=Screen::Journal;
            if(Action("Preciso de uma orientação"))openMenu(Screen::Help);
            if(Action("Afastar-se do rádio"))investigation.screen=Screen::Explore;break;
        case Screen::LensConsole: {
            Heading("MECANISMO DA LENTE","Bancada de reconstituição");
            Paragraph("Confira como o sinal poderia ter sido produzido. Este é um teste local de bancada: ele não transmite uma mensagem nem altera o registro histórico.");
            const bool recordsReady=investigation.powerSolved&&investigation.DoorOpen()&&investigation.radioSolved&&investigation.discovered[4]&&investigation.discovered[8];
            const bool locked=investigation.lensSolved||!recordsReady;
            const auto p=ImGui::GetCursorScreenPos();const float w=ImGui::GetContentRegionAvail().x;auto* d=ImGui::GetWindowDrawList();
            ImGui::Dummy(ImVec2(w,112));
            d->AddRectFilled(p,ImVec2(p.x+w,p.y+112),Ink(5,16,18),4.f);
            d->AddRect(p,ImVec2(p.x+w,p.y+112),Ink(138,159,144,.35f),4.f);
            Letter(d,ImVec2(p.x+18,p.y+15),10.5f,Ink(146,176,164),"COMANDO ÓPTICO / ENSAIO ISOLADO");
            const float span=(w-36.f)/3.f;
            for(int i=0;i<3;++i){
                const float x=p.x+18.f+i*span;
                const float length=span*(investigation.lensLong[i]?.77f:.31f);
                d->AddRectFilled(ImVec2(x,p.y+48),ImVec2(x+length,p.y+61),Ink(211,181,118,recordsReady?.80f:.28f),2.f);
                const std::string name=std::string("PULSO ")+std::to_string(i+1);
                Letter(d,ImVec2(x,p.y+77),10.5f,Ink(142,168,167),name.c_str());
            }
            ImGui::Spacing();
            if(lensPlaybackTime>=0){
                const int current=(std::clamp)(int((lensPlaybackTime-ultimo::LensLeadIn)/ultimo::LensPulseSpacing)+1,1,3);
                ImGui::TextColored(gold,"TESTE LOCAL: pulso %d de 3 — LONGO",current);
                ImGui::ProgressBar((std::clamp)(lensPlaybackTime/ultimo::LensPlaybackDuration,0.f,1.f),ImVec2(-1,5),"");
            }
            if(!recordsReady)Paragraph("Ainda falta contexto para conferir o ensaio. Reúna o registro mecânico, a fotografia e a comunicação de Brumas; eles permanecem disponíveis no caderno.");
            if(investigation.lensSolved){ImGui::TextColored(gold,"ENSAIO CONFERIDO");Paragraph("A configuração e a sequência foram registradas no caderno. Agora relacione o teste com o que aconteceu naquela noite.");}
            ImGui::BeginDisabled(locked);
            ImGui::TextColored(muted,"MODO DO COMANDO");
            if(ImGui::RadioButton("Automático",!investigation.lensManual)){investigation.lensManual=false;audio.Cue(1);Save();}
            ImGui::SameLine();if(ImGui::RadioButton("Manual",investigation.lensManual)){investigation.lensManual=true;audio.Cue(1);Save();}
            ImGui::Spacing();ImGui::TextColored(muted,"SEQUÊNCIA DO ENSAIO");
            if(ImGui::BeginTable("##lens-pulses",3,ImGuiTableFlags_SizingStretchSame)){
                for(int i=0;i<3;++i){
                    ImGui::TableNextColumn();ImGui::PushID(i);ImGui::Text("Pulso %d",i+1);
                    if(ImGui::Button(investigation.lensLong[i]?"Longo":"Curto",ImVec2(-1,38))){investigation.lensLong[i]=!investigation.lensLong[i];audio.Cue(1);Save();}
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
            ImGui::Spacing();
            if(Action("Reconstituir sinal",true)){if(investigation.VerifyLens())StartLensPlayback();else audio.Cue(4);Save();}
            ImGui::EndDisabled();
            if(investigation.lensSolved){ImGui::BeginDisabled(lensPlaybackTime>=0);if(Action("Reproduzir teste local"))StartLensPlayback();ImGui::EndDisabled();}
            if(!investigation.notice.empty())Paragraph(investigation.notice.c_str());
            if(investigation.discovered[8]&&Action("Consultar registro mecânico")){inspectedEvidence=-1;investigation.readingEntry=8;investigation.readingReturn=Screen::LensConsole;investigation.screen=Screen::Reading;}
            if(Action("Consultar caderno"))investigation.screen=Screen::Journal;
            if(Action("Preciso de uma orientação"))openMenu(Screen::Help);
            if(Action("Afastar-se do comando"))investigation.screen=Screen::Explore;break;
        }
        case Screen::Help:
            Heading("AJUDA SOB PEDIDO","Um passo de cada vez");Paragraph(investigation.Question());
            if(investigation.helpStage==0){Paragraph("Você escolhe o quanto quer revelar. Comece por uma orientação; a solução direta só aparece se você pedir.");
                if(Action("Mostrar orientação sutil",true))investigation.helpStage=1;
            }else{
                ImGui::TextColored(gold,"%s",investigation.helpStage==1?"ORIENTAÇÃO":investigation.helpStage==2?"INDICAÇÃO MAIS ESPECÍFICA":"SOLUÇÃO SOLICITADA");Paragraph(investigation.Hint());
                if(investigation.helpStage==1&&Action("Quero uma indicação mais específica"))investigation.helpStage=2;
                else if(investigation.helpStage==2&&Action("Revelar a solução deste desafio"))investigation.helpStage=3;
            }
            ImGui::Spacing();if(Action("Voltar à investigação"))returnMenu();break;
        case Screen::Options:
            Heading("AJUSTES","Faça a ilha ficar legível");DrawSettings();
            ImGui::Spacing();if(Action("Aplicar e voltar",true)){SaveSettings();returnMenu();}break;
        case Screen::Controls: {
            Heading("CONTROLES","Movimento e observação");Paragraph("Mouse: olhar. Clique em uma tecla abaixo e pressione uma letra de A a Z. Esc cancela a troca.");
            const char* names[]{"Avançar","Recuar","Mover à esquerda","Mover à direita","Examinar / guardar leitura","Abrir / fechar caderno"};
            const int keys[]{settings.forward,settings.back,settings.left,settings.right,settings.interact,settings.journal};
            if(ImGui::BeginTable("##bindings",2,ImGuiTableFlags_SizingStretchProp)){
                ImGui::TableSetupColumn("Ação",ImGuiTableColumnFlags_WidthStretch,2.f);ImGui::TableSetupColumn("Tecla",ImGuiTableColumnFlags_WidthStretch,1.f);
                for(int i=0;i<6;++i){ImGui::PushID(i);ImGui::TableNextRow();ImGui::TableNextColumn();ImGui::TextWrapped("%s",names[i]);ImGui::TableNextColumn();
                    if(ImGui::Button(remapping==i?"Pressione uma tecla":KeyLabel(keys[i]).c_str(),ImVec2(-1,32)))remapping=i;ImGui::PopID();}
                ImGui::EndTable();
            }
            ImGui::Spacing();Paragraph("Tab / Esc: pausa e retorno. F1: ajuda gradual. F2: opções. Enter: preparar o relatório quando a investigação estiver fundamentada.");
            ImGui::TextDisabled("Teclas em uso ou reservadas não podem ser repetidas.");
            if(Action("Restaurar controles")){settings.forward='W';settings.back='S';settings.left='A';settings.right='D';settings.interact='E';settings.journal='J';remapping=-1;SaveSettings();}
            if(Action("Aplicar e voltar",true)){remapping=-1;SaveSettings();returnMenu();}break;
        }
        case Screen::ConfirmNew:
            Heading("NOVA INVESTIGAÇÃO","Substituir o progresso salvo?");Paragraph("Uma nova investigação apaga suas descobertas, mecanismos e relatório atuais. As opções de som, imagem e controles serão mantidas.");
            if(Action("Manter meu progresso",true))returnMenu();
            if(Action("Apagar progresso e começar")){DeleteSave();Start();}break;
        case Screen::Conclusion:
            Heading("RELATÓRIO DE CAMPO","O que aconteceu depois da queda?");
            Paragraph("Escolha a explicação sustentada pelo que encontrou. Relacione pelo menos duas evidências e registre sua justificativa antes de consultar o resultado.");
            for(int i=0;i<int(ultimo::Hypotheses.size());++i){ImGui::PushID(i);ImGui::RadioButton("##hypothesis",&investigation.hypothesis,i);ImGui::SameLine();ImGui::PushTextWrapPos();ImGui::TextUnformatted(ultimo::Hypotheses[i]);ImGui::PopTextWrapPos();ImGui::PopID();}
            ImGui::Separator();ImGui::TextColored(gold,"EVIDÊNCIAS QUE APOIAM SUA RESPOSTA");
            for(int i=0;i<int(ultimo::EntryCount);++i)if(investigation.discovered[i])ImGui::Checkbox(ultimo::Entries[i].name,&investigation.evidence[i]);
            ImGui::Spacing();ImGui::TextUnformatted("Sua justificativa:");
            ImGui::InputTextMultiline("##justification",answer.data(),answer.size(),ImVec2(-1,110));investigation.justification=answer.data();
            ImGui::BeginDisabled(!investigation.CanSubmit());if(Action("Transmitir relatório",true)){if(investigation.Submit()){audio.Cue(0);Save();}}ImGui::EndDisabled();
            if(Action("Continuar investigando")){Save();investigation.screen=Screen::Explore;}break;
        default:break;
        }
        if(!saveNotice.empty()){ImGui::Separator();ImGui::TextWrapped("%s",saveNotice.c_str());}ImGui::End();
    }
    ImGui::PopStyleVar(5);ImGui::PopStyleColor(12);SyncCursor();
}

void UltimoSinal::DrawSettings() {
    ImGui::TextColored(gold,"SOM");ImGui::SliderFloat("Volume geral",&settings.master,0.f,1.f,"%.2f");
    ImGui::SliderFloat("Mar e ambiente",&settings.ambient,0.f,1.f,"%.2f");ImGui::SliderFloat("Passos e equipamentos",&settings.effects,0.f,1.f,"%.2f");
    if(!audio.Available())ImGui::TextWrapped("Não há dispositivo de áudio disponível. As informações essenciais também aparecem por escrito.");
    ImGui::Spacing();ImGui::TextColored(gold,"CÂMERA");
    ImGui::SliderFloat("Sensibilidade",&settings.sensitivity,.015f,.20f,"%.3f",ImGuiSliderFlags_Logarithmic);
    ImGui::Checkbox("Inverter olhar vertical",&settings.invertY);ImGui::Checkbox("Reduzir movimentos de câmera",&settings.reduceMotion);
    ImGui::Spacing();ImGui::TextColored(gold,"IMAGEM");ImGui::SliderFloat("Brilho",&settings.brightness,.75f,1.35f,"%.2f");
    ImGui::TextWrapped("Ajuste até distinguir a superfície escura da parede, sem perder o contraste da luz.");
    float sampleWidth=(std::max)(30.f,ImGui::GetContentRegionAvail().x/5.f-8.f);
    for(int i=0;i<5;++i){float v=(.04f+.045f*i)*settings.brightness;
        ImGui::ColorButton((std::string("##shade")+std::to_string(i)).c_str(),ImVec4(v,v,v,1),ImGuiColorEditFlags_NoTooltip|ImGuiColorEditFlags_NoDragDrop,ImVec2(sampleWidth,25));if(i<4)ImGui::SameLine();}
    ImGui::Combo("Qualidade",&settings.quality,"Baixa\0Média\0Alta\0");
    ImGui::TextDisabled("A qualidade ajusta o detalhamento e o custo dos efeitos.");
    ImGui::Spacing();if(Action("Restaurar opções padrão"))settings.Reset();
}
