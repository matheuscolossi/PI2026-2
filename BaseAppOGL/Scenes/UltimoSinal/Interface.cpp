#include "UltimoSinal.h"
#include <algorithm>
using ultimo::Screen;using ultimo::ClueId;
void UltimoSinal::DrawInterface() {
    const auto display=ImGui::GetIO().DisplaySize;
    const auto fixed=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoCollapse;
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.04f,.08f,.11f,.96f));
    ImGui::PushStyleColor(ImGuiCol_Button,ImVec4(.15f,.35f,.36f,1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,ImVec4(.22f,.46f,.44f,1));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(20,16));
    if(investigation.screen==Screen::Explore) {
        ImGui::SetNextWindowPos({18,18});ImGui::SetNextWindowBgAlpha(.88f);
        ImGui::Begin("O ÚLTIMO SINAL",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("Pistas: %zu / 5",investigation.Count());
        ImGui::TextUnformatted(investigation.DoorOpen()?"Explore a sala da lente.":"Entre no farol e encontre a chave.");
        ImGui::Separator();ImGui::TextUnformatted("WASD: andar | Mouse: olhar | E: examinar");
        ImGui::TextUnformatted("J: caderno | Tab: pausa | Esc: sair");
        if(investigation.AllFound())ImGui::TextUnformatted("Enter: apresentar sua conclusão.");
        ImGui::End();
        ImGui::GetForegroundDrawList()->AddCircleFilled({display.x*.5f,display.y*.5f},3,IM_COL32(255,233,184,255));
        if(target>=0) {
            ImGui::SetNextWindowPos({display.x*.5f,display.y*.82f},ImGuiCond_Always,{.5f,0});
            ImGui::Begin("Examinar",nullptr,fixed|ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("E - %s",ultimo::ClueTexts[target].name);ImGui::End();
        }
    } else {
        ImGui::SetNextWindowPos({display.x*.5f,display.y*.5f},ImGuiCond_Always,{.5f,.5f});
        ImGui::SetNextWindowSize({(std::min)(660.f,display.x-32),0});
        ImGui::SetNextWindowSizeConstraints({0,0},{display.x-32,display.y-48});
        ImGui::Begin("O ÚLTIMO SINAL",nullptr,fixed|ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::SetWindowFontScale(1.2f);
        if(investigation.screen==Screen::Title) {
            ImGui::TextUnformatted("Uma ilha. Um farol. Cinco pistas.");ImGui::Separator();
            ImGui::TextWrapped("%s",ultimo::Intro);
            ImGui::Spacing();
            if(ImGui::Button("Iniciar investigação  [Enter]",{-1,38}))Start();
            ImGui::TextDisabled("Ilha das Brumas | Uma investigação sobre a morte de Augusto");
            DrawSettings();
            ImGui::TextUnformatted("WASD / Mouse / E / J / Tab | Esc: sair");
        } else if(investigation.screen==Screen::Reading) {
            const auto& clue=ultimo::ClueTexts[static_cast<std::size_t>(investigation.reading)];
            ImGui::TextUnformatted(clue.name);ImGui::Separator();ImGui::TextWrapped("%s",clue.description);
            ImGui::Spacing();if(ImGui::Button("Voltar  [E]",{-1,32}))investigation.CloseReading();
        } else if(investigation.screen==Screen::Journal) {
            ImGui::Text("CADERNO DE EVIDÊNCIAS | %zu de 5 pistas",investigation.Count());ImGui::Separator();
            for(std::size_t i=0;i<ultimo::ClueCount;++i) {
                auto id=static_cast<ClueId>(i);ImGui::PushID(int(i));
                if(investigation.Found(id)){if(ImGui::Button(ultimo::ClueTexts[i].name,{-1,30}))investigation.ReadAgain(id);}
                else ImGui::TextDisabled("[ ] %s",ultimo::ClueTexts[i].name);
                ImGui::PopID();
            }
            ImGui::Spacing();if(ImGui::Button("Fechar caderno  [J]",{-1,32}))investigation.ToggleJournal();
        } else if(investigation.screen==Screen::Pause) {
            ImGui::TextUnformatted("PAUSADO");ImGui::Separator();
            if(ImGui::Button("Continuar  [Tab]",{-1,32}))investigation.TogglePause();
            if(ImGui::Button("Reiniciar investigação  [R]",{-1,32}))Start();
            DrawSettings();
            ImGui::TextWrapped("Aulas preservadas: teclas 0, 1, 5 e 6. Tecla 2: reinicia a cena do jogo.");
        } else if(investigation.screen==Screen::Conclusion) {
            ImGui::TextUnformatted("SEU RELATÓRIO");ImGui::Separator();
            ImGui::TextWrapped("O que melhor explica a morte de Augusto e o último sinal?");
            for(int i=0;i<int(ultimo::Hypotheses.size());++i){
                ImGui::PushID(i);ImGui::RadioButton("##hipotese",&investigation.hypothesis,i);
                ImGui::SameLine();ImGui::PushTextWrapPos();ImGui::TextUnformatted(ultimo::Hypotheses[i]);ImGui::PopTextWrapPos();ImGui::PopID();
            }
            ImGui::Separator();ImGui::TextUnformatted("Marque pelo menos duas evidências:");
            for(std::size_t i=0;i<ultimo::ClueCount;++i)ImGui::Checkbox(ultimo::ClueTexts[i].name,&investigation.evidence[i]);
            ImGui::TextUnformatted("Explique sua conclusão antes de consultar o resultado:");
            ImGui::InputTextMultiline("##justificativa",answer.data(),answer.size(),{-1,85});
            investigation.justification=answer.data();
            ImGui::BeginDisabled(!investigation.CanSubmit());
            if(ImGui::Button("Entregar relatório e ver explicação",{-1,34}))investigation.Submit();
            ImGui::EndDisabled();
            if(ImGui::Button("Voltar à investigação",{-1,28}))investigation.screen=Screen::Explore;
        } else if(investigation.screen==Screen::Complete) {
            ImGui::TextUnformatted(investigation.Correct()?"AS EVIDÊNCIAS SUSTENTAM SUA HIPÓTESE":"REVEJA A RELAÇÃO ENTRE AS EVIDÊNCIAS");
            ImGui::Separator();ImGui::TextWrapped("%s",ultimo::Explanation);
            if(ImGui::CollapsingHeader("Seu relatório, registrado antes do feedback")){
                ImGui::TextWrapped("%s",ultimo::Hypotheses[investigation.hypothesis]);
                for(std::size_t i=0;i<ultimo::ClueCount;++i)if(investigation.evidence[i])ImGui::BulletText("%s",ultimo::ClueTexts[i].name);
                ImGui::TextWrapped("%s",investigation.justification.c_str());
            }
            ImGui::Separator();ImGui::TextWrapped("Você transmite o relatório. Após alguns segundos de estática, uma voz desconhecida pergunta: ‘O farol já pode voltar a acender?’");
            ImGui::Text("Tempo de sessão: %.0f s (inclui leitura e pausas)",duration);
            ImGui::TextDisabled("A justificativa requer avaliação humana; não é corrigida automaticamente.");
            if(ImGui::Button("Reiniciar  [R]",{-1,32}))Start();
        }
        ImGui::End();
    }
    ImGui::PopStyleVar();ImGui::PopStyleColor(3);SyncCursor();
}
void UltimoSinal::DrawSettings() {
    if(ImGui::CollapsingHeader("Opções de câmera")){
        ImGui::SliderFloat("Sensibilidade",&settings.sensitivity,.015f,.20f,"%.3f",ImGuiSliderFlags_Logarithmic);
        ImGui::Checkbox("Inverter movimento vertical",&settings.invertY);
        if(ImGui::Button("Restaurar padrão"))settings.Reset();
        ImGui::TextDisabled("Ajustes mantidos durante esta sessão.");
    }
    if(ImGui::CollapsingHeader("Comparar efeitos gráficos")){
        ImGui::Checkbox("Iluminação Point",&pointEnabled);ImGui::Checkbox("Iluminação Spot",&spotEnabled);
        ImGui::Checkbox("Multitextura do terreno",&multiEnabled);ImGui::Checkbox("Normal mapping da pedra",&normalEnabled);
        ImGui::Checkbox("Reflexão / refração do céu",&environmentEnabled);ImGui::Checkbox("Neblina",&fogEnabled);
    }
}
