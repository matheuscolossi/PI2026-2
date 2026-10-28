# O Último Sinal — revisão de experiência, 21/10/2026

## Mudanças que já estão no jogo

- Menu inicial sobre a cena 3D, com câmera de apresentação e navegação por teclado.
- Prólogo de 13 segundos, com chegada à ilha, textos em sequência e opção de pular.
- Quatro capítulos da investigação e epílogo, derivados do progresso real já salvo.
- Objetivo contextual, avisos de descoberta e retomada no capítulo correspondente.
- Armário com abertura gradual após a energia; porta, câmera e animações congelam na pausa.
- Interface própria para quadro elétrico e rádio; encerramento visual separado do relatório.
- Água com ondulações/espuma, luz giratória após alimentação e partículas discretas nas luminárias.
- Iluminação com ambiente frio e pontos quentes, exposição e resposta de superfície por tipo de material.
- 19 exportações nativas atualizadas no 3ds Max: bordas de móveis e equipamentos arredondadas, cilindros com mais lados e UVs refeitas.
- A mesa mantém cinco Boxes editáveis; Chamfer e Unwrap acrescentam acabamento. O export tem 2.940 triângulos e conserva 1,60 × 1,00 × 0,90 m.
- Piso corrigido: o export de um nó estava em coordenadas locais e ficava sob a fundação. A origem (0; 0,075; −0,5) devolve o topo a Y=0,095.
- Novo CompleteMap nativo do piso, UV1 de 1024 × 1024. Somente o piso é baked; outras superfícies mantêm luzes/sombras analíticas.

## Fluxo para demonstrar

Abra Jogar.cmd. Nova investigação mostra a chegada; Enter pula a introdução.
WASD move, mouse observa, E examina e J abre o caderno. Tab/Esc pausa.
As transições de capítulo acompanham energia, chave e investigação fundamentada.
Para rever o prólogo usando uma partida existente, use Nova investigação e confirme a substituição somente se quiser perder aquela partida.
Continuar investigação recupera o progresso existente sem repetir descobertas.

## Conferências executadas

Release e Debug: 188 verificações de execução, zero falhas; 37 modelos do cenário aceitos, zero fallback e cinco instâncias da mesa.
As cenas 0, 1, 5, 6 e 2 continuaram desenhando com GL0. Foram exportados 27 quadros reais do renderizador, incluindo menu, prólogo, capítulos e armário em diferentes posições.
Lógica anterior: 436 verificações e 184 estados alcançáveis. Nova apresentação: 42 verificações de capítulo, introdução, pausa, fila de eventos e retomada.
O teste de execução cobre também o percurso até a chave e o encerramento da investigação.
Prévia pela janela real: menu, Tab/Enter para iniciar, introdução, pausa/retomada e entrada na exploração observados. Hashes da partida e das opções reais permaneceram iguais no perfil isolado.
Na prévia final, um único Enter iniciou o prólogo corretamente. O perfil real continuou com os mesmos hashes.

O Master foi salvo e reaberto no Max. A auditoria permanece em 37 grupos/628 componentes, todos com Unwrap; logs nativos e dados de importação estão preservados.
A comparação nativa de sombras on/off do novo bake alterou 26.304 pixels acima de 12 níveis. Isso comprova o elemento bake comparado, sem afirmar que todo o cenário foi baked.

## Limites reais

Esta revisão melhora a apresentação do jogo; o cenário continua compacto, com duas salas e exterior. Os capítulos organizam a mesma investigação e não representam mapas adicionais.
Não foram produzidos personagens animados, animações de mãos, dublagem, multiplayer ou cinemáticas com atores. O movimento de ambiente e os eventos usam código.
Reflexos continuam usando o cubemap da base. O feixe no ar usa uma superfície transparente suave, sem ray tracing volumétrico. Os materiais não são um conjunto PBR medido.
A duração com jogadores, o roteiro humano completo e o computador da apresentação continuam pendentes. Testes internos não substituem avaliação com participantes nem tornam o jogo profissional por declaração.

## Fontes e organização

A estrutura C++/OpenGL e as cenas do professor foram mantidas. A produção do Max usa scripts nativos assistidos por Codex; a madeira continua proveniente da aula.
O `.max` é fonte editável; o `.3ds` e seus bitmaps entram no runtime. As evidências ficam no pacote de fontes separado.
Experiência: Experience.h/Experience.cpp. Fluxo: UltimoSinal.cpp. Interface: Interface.cpp. Mundo/material: World.cpp e Prototype.frag/Sky.frag.
Logs atuais: logs/pro_upgrade_2026-10-21; produção nativa: Scenes/Models/Cenario/evidencias/polish_native.txt.
O modificador foi usado segundo a [referência oficial Chamfer da Autodesk](https://help.autodesk.com/cloudhelp/2026/ENU/MAXScript-Help/files/3ds-Max-Objects-and-Interfaces/Modifier-MAXWrapper-and/Modifier-and-SpacewarpModifier/Modifiers/Mesh-Editing/GUID-BC52D8A4-E491-4AF8-B0FC-CB1A07561EF4.html).
