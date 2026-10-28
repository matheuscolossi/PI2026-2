## Revisão atual — 22/10/2026: cenário e percurso vertical

Escada em espiral com 36 degraus transitáveis, abertura real na cobertura, patamar e sala óptica acessível. Três áreas conectadas no mesmo cenário: sala de serviço, arquivo e sala óptica. Acesso ao armário liberado, móveis reposicionados e 14 exports do Max atualizados, incluindo rádio, escrivaninha, diário, cadeira e fotografia. Conjunto: 39 modelos do cenário sem fallback mais mesa.3ds (40 arquivos); quatro mesas e uma escrivaninha.

Release/Debug e executável instalado: 306 verificações por execução, zero falhas. Lógica: 3.446/1.632 estados; apresentação: 46; áudio PCM: 38; suporte espacial: 185. Há 38 quadros de revisão e cinco cenas sem erro GL. Apenas o piso recebeu novo CompleteMap. Madeira e fotografia geradas são declaradas em CREDITOS.md. Progresso real preservado.

Procedimento, apresentação, alterações no código, evidências e pendências: [Revisão do cenário](Revisao_cenario_22-10-2026.md). Os blocos datados abaixo são históricos; seus totais e posições antigas não descrevem a versão atual.

## Revisão atual — 21/10/2026: investigação e inspeção

Três enigmas conectados: energia, rádio e novo ensaio local do comando da lente. Foram integrados console físico, inspeção 3D manipulável de quatro evidências, caderno agrupado com horários conhecidos, volumes de interação e áudio por distância/direção. Rascunhos e confirmações do console são salvos; partidas antigas incompletas preservam descobertas e relatórios já concluídos permanecem imutáveis.

Release/Debug e executável instalado: 218 verificações, zero falhas; lógica: 3.446 verificações/1.632 estados; apresentação: 46; áudio PCM: 38. Há 33 quadros do renderizador, 37 modelos do cenário aceitos sem fallback mais a mesa e cinco cenas de aula sem erro GL. Prévia dos controles feita em perfil isolado, sem alterar o progresso real. Ainda há acabamento artístico e avaliação com jogadores pendentes.

Guia, explicação do código, validação e limites: [Revisão profunda](Revisao_profunda_21-10-2026.md). Os registros datados abaixo preservam etapas anteriores e seus totais antigos.

# Verificações — 20/10/2026

## Conferência final do conjunto do Max em 20 de outubro

Release e Debug foram compilados e executados: 178 verificações em cada configuração, zero falhas, 37 recursos do cenário aceitos e zero fallback. A mesa original permanece reutilizada em cinco instâncias. As cenas 0, 1, 5, 6 e 2 desenharam sem erro OpenGL. Foram exportados 18 quadros de revisão.

O Master cenario.max foi salvo e reaberto no Max. A auditoria dos 37 grupos registrou 628 componentes, todos com Unwrap UVW; o template nativo piso_uv.png foi exportado e conferido. O piso usa CompleteMap Scanline 1024 x 1024 em UV1, sem relighting Point/Spot. As demais geometrias mantêm luzes e sombras analíticas. A comparação nativa de sombras alterou 13.922 pixels acima de 12 níveis.

O pacote jogável foi gerado e executado a partir de pasta de trabalho externa, com 178 verificações sem falhas. O teste negativo de recurso ausente retornou código 2. Os 38 modelos, seus bitmaps e o executável mantiveram os hashes entre origem, distribuição e ZIP. As versões anteriores foram preservadas em dist/backups. Fontes editáveis e evidências estão separadas em Cenario-3dsMax-Fontes.zip.

O teste curto de renderização teve 2 s de aquecimento e 3 s de coleta em 1280 x 720 com MSAA4: 1.115 quadros, mediana 2,4915 ms e p95 4,4612 ms na RTX 3050. É sanidade técnica; continuam pendentes a medição integral atual, a janela no computador da apresentação e a experiência com pessoas.

Pendências reais: polimento visual, percurso humano completo, controles em diferentes DPIs, técnicas e transparências em movimento, áudio, execução em outro computador e piloto com participantes. Não houve novo envio ao Git nem submissão acadêmica nesta rodada.

Evidências: logs/native_bake_runtime_release.log, logs/native_bake_runtime_debug.log, logs/native_bake_benchmark_smoke.log, logs/mesa_package_verificacao.json e Scenes/Models/Cenario/evidencias. Os totais históricos abaixo não devem ser somados aos atuais.

Pacote atual: 51,491,195 bytes; SHA-256 `4f41c8531e92a5668b51028fdb92922ce7b9a53178bf50cad6330dadfb5605e4`.

Este relatório corresponde à revisão com enigmas de alimentação e rádio, dez registros, salvamento, opções persistentes, áudio procedural e cenário revisado.

## Compilação

As configurações **Debug x64** e **Release x64** foram compiladas no Visual Studio Community 2026, com toolset v145 e bibliotecas correspondentes do SDK local.

- Debug: `BaseAppOGL/bin/Debug/BaseAppOGL.exe`.
- Release da revisão: `BaseAppOGL/bin/InvestigacaoRelease/BaseAppOGL.exe`.
- Logs: `logs/deep_Debug.log` e `logs/deep_Release.log`.

As saídas Debug e Release ficam separadas para evitar sobrescrita entre configurações. Compilar não demonstra por si só qualidade visual, conforto dos controles ou conformidade de autoria no Max.

## Testes puros

**436 verificações passaram.** A exploração das regras encontrou **184 estados de progresso alcançáveis**, todos com caminho para completar a investigação. Foram abrangidas as **oito combinações dos três ramais** e os **16 canais do rádio**.

Esses testes cobrem pré-requisitos dos enigmas, descoberta e releitura, conclusão, resposta anterior ao feedback, geometria, mouse, consistência do estado e armazenamento. Dez verificações adicionadas nesta etapa protegem o relatório enviado contra edição posterior ao feedback. A análise dos estados verifica as regras abstratas; não significa que uma pessoa percorreu fisicamente todas as ordens possíveis no cenário.

Evidências: `logs/deep_tests_result.txt` e `logs/deep_tests.log`. Para repetir, execute `tools/Testar.ps1` ou compile `tests/Testes.vcxproj`.

## Verificação no executável

A revisão de cinco mesas passou em **58 verificações sem falhas**, no modo `--verify-game`; cinco cenas carregaram e desenharam com GL0. O registro atual é `logs/mesas_cinco_runtime.log`. As rodadas anteriores de 47 e 55 verificações são históricas e não devem ser somadas ao total atual. O executável Release desta rodada tem SHA-256 `9f6e8810fad8793657ed0cb84fa020076d1c3ddbc98a7b07dbf3259d30c53ba7`.

A verificação cobre:

- Salvamento e continuação no início.
- Opções, Controles e Ajuda anteriores a uma sessão ativa não gravam progresso (três regressões adicionais).
- Porta bloqueada antes da chave e alvo atrás da porta indisponível.
- Folha da porta permanecendo sólida quando aberta.
- Percurso por posições reais, com colisões.
- Alvos acessíveis pela seleção real, sem atravessar paredes.
- Alimentação rearmada após consultar as evidências.
- Salvamento e continuação após recuperar energia.
- Chave liberando a passagem.
- Sintonia correta e registro da transcrição.
- Registro mecânico e fotografia permitindo o relatório.
- Envio de hipótese/justificativa e recuperação do desfecho; o relatório enviado permanece consultável, sem reabrir edição.
- Arquivo corrompido recusado sem apagar o estado da sessão.
- Persistência de opções em disco.
- Reinício limpando chave e mecanismos.

A aplicação utiliza `ULTIMO_TEST_HOME` em diretório temporário isolado para verificação e benchmark. Assim, os testes automatizados não usam o progresso habitual de `%LOCALAPPDATA%/UltimoSinal`.

O rascunho tem salvamento periódico de aproximadamente cinco segundos, inclusive durante a digitação, e a cena solicita salvamento ao ser destruída. Uma falha de gravação não remove a sessão em memória: Continuar e a confirmação de novo jogo consideram essa sessão. A recuperação em memória não sobrevive ao encerramento e deve ser distinguida da persistência em disco.

O percurso automatizado move a câmera e aponta para alvos por código. Ele confirma acessibilidade geométrica para o percurso testado, mas não mede facilidade de encontrar os objetos com mouse e teclado, conforto ou compreensão dos enigmas.

## Primeiro modelo do Max confirmado e integrado

Arquivos em `BaseAppOGL/Scenes/Models/Mesa`:

| Arquivo | Tamanho confirmado | Função |
|---|---:|---|
| `mesa.max` | 774.144 bytes | Fonte editável após conferência e novo salvamento no Max |
| `mesa.3ds` | 5.800 bytes | Exportação 3D Studio carregada pelo jogo |
| `wood.jpg` | 915.211 bytes | Cópia de P1.jpg da aula |

O log `evidencias/producao_max.txt` identifica **3ds Max 2027, ENU/English**, unidades em metros, cinco Boxes, 12 triângulos/24 vértices UV por peça e modificador Unwrap UVW preservado. Os scripts nativos aplicam Flatten Mapping e material Standard com Bitmap no Diffuse. O processo foi assistido pelo Codex por MAXScript executado no Max.

O log de produção preserva a falha inicial ao selecionar o tipo de viewport e a mensagem prematura sobre o template UV. A recuperação e a conferência posterior ficaram registradas: em 20/10/2026, `verificar_mesa.ms` foi executado no MAXScript Listener do Max e reabriu o arquivo salvo, confirmando cinco Boxes, Unwrap UVW e Standard. Trata-se de reabertura nativa assistida por MAXScript.

`evidencias/checker.png` mostra as cinco peças com quadrados proporcionais; a densidade varia entre tampo e pernas. A madeira foi restaurada antes do salvamento/reabertura, e o `.3ds` não recebeu o checker. O template UV nativo do tampo, de **1024 × 1024**, foi exportado pelo Max em caminho ASCII do workspace, copiado byte a byte para `evidencias/uv.png` e inspecionado. Também existem `uv_editor.jpg` e `modelo_editor.jpg`. O log `conferencia_max.txt` e o resumo `logs/mesa_producao_verificada.json` registram a confirmação e os hashes. O `.max` atual tem SHA-256 `599f8444b4a474caf88c7a7d49537d6278b447e53584a1ce6d791b4f42d4a9d2`.

A integração Release confirmou **uma malha combinada, 60 triângulos, dimensões 1,60 × 1,00 × 0,90 m em X/Y/Z, UVs/normais finitos e um Bitmap difuso carregado**. Cinco peças no Max podem ser combinadas pelo importador; uma malha não significa que apenas uma caixa foi produzida.

Um único `CAsset` é desenhado em **cinco instâncias**: diário (−3; 3), manutenção (−3; 1), registro (−3; −3), fotografia (3; −3) e rádio (3; 2,5), coordenadas X/Z. Os pés apoiam em Y=0,095. Quatro mantêm 1,60 × 1,00 × 0,90 m; o rádio usa escala em Z para profundidade 1,95 m. Os `.max`/`.3ds` de origem não mudaram. Cada desenho reutiliza 60 triângulos; cinco instâncias não transformam o asset em cinco malhas/modelos autorais.

O material usa as UVs importadas, e as mesmas instâncias definem desenho, colisão e sombra. O percurso verificou quantidade, posições, profundidade da bancada, matrizes/caixas, apoio e acesso às pistas. Os quadros 08, 09 e 10 documentam fotografia, sala inicial e rádio. Não permanece a limitação da primeira integração que substituía apenas a mesa da fotografia.

Geometria e UV são a primeira produção parcial no Max do projeto, com assistência declarada. `wood.jpg` e `Scenes/Textures/P1.jpg` têm SHA-256 idêntico, `2a6b31601595f94d4d33f24095be9611b45f364d692051c4ed8eb307d3c1145b`: a imagem é do material da aula. Esta era a situação anterior ao conjunto completo; a conferência atual está no início deste documento.

## Renderização e cenas acadêmicas

As cenas **0, 1, 2, 5 e 6** foram carregadas e renderizadas com **erro OpenGL igual a zero** no teste integrado. Erros de carregamento e de desenho são verificados separadamente.

O renderizador exportou **dez quadros próprios** para revisão:

1. `01_exterior.bmp`: aproximação externa e composição do farol.
2. `02_service_room.bmp`: sala de serviço.
3. `03_lens_room.bmp`: sala da lente.
4. `04_options.bmp`: opções.
5. `05_diary.bmp`: leitura.
6. `06_report.bmp`: relatório.
7. `07_front_oblique.bmp`: vista oblíqua para conferir a correção de faces coplanares.
8. `08_mesa_importada.bmp`: mesa importada da fotografia.
9. `09_mesas_sala_inicial.bmp`: mesas do diário e da manutenção.
10. `10_bancada_radio.bmp`: bancada do rádio com profundidade adaptada.

Os arquivos ficam em `logs/deep_frames`; a rodada atual está em `logs/mesas_cinco_runtime.log`. Imagens estáticas ajudam a conferir composição, UV/material, posição e ausência de duplicação, mas não demonstram todos os ângulos, qualidade em movimento ou uso humano dos controles.

## Verificação autorizada da interface na janela

Após autorização do usuário e criação de backups, a interface foi operada pela ferramenta Sky. O bloqueio inicial de acesso deixou de ser um impedimento para esta verificação. Também foi corrigido e testado o guard de salvamento: abrir Opções, Controles ou Ajuda antes de iniciar/continuar uma sessão não grava progresso novo.

Foram conferidos os seguintes comportamentos:

- No início, abrir Opções; alterar brilho de 1,00 para 1,20; encerrar e reabrir o aplicativo; confirmar 1,20 persistido.
- Alterar sensibilidade de 0,060 para 0,079 e retornar a 0,060.
- Alternar qualidade Alta → Média → Alta; percorrer a rolagem até o botão Aplicar.
- Em Controles, tentar atribuir W ao comando Examinar e receber recusa por duplicidade; atribuir Q, reabrir o aplicativo e confirmar Q persistido.
- Restaurar Examinar em E e movimento/caderno em WASD/J. Ao terminar, o arquivo de opções ficou byte a byte igual ao backup original.
- Solicitar Nova investigação e escolher Manter meu progresso.
- Usar Continuar para recuperar a posição exterior; abrir e fechar o caderno vazio com J.
- Abrir ajuda com F1 no estágio inicial, sem solução revelada, e retornar com Esc.
- Pausar e retomar com Tab, sem salto aparente da câmera nas imagens observadas; usar Sair na pausa para encerrar.

O hash de `progress.v3` permaneceu igual ao backup durante os menus e reinícios anteriores a Continuar. Após carregar, salvamentos do tempo/câmera eram esperados e não representam apagamento de pistas. Os backups estão no workspace em `work/ui_backup_2026-10-19`.

Essas ações comprovam o funcionamento observado dos menus, da persistência e das transições conferidas. **Não constituem percurso humano completo**. A interação Q em um objeto do mundo não foi conferida: não havia alvo próximo. Toques de teclas sem manter movimento não foram usados para afirmar teste de caminhada ou colisão pela interface. Essas partes continuam apoiadas no percurso automatizado, ampliado para 58 verificações na rodada de cinco instâncias.

O áudio não foi ouvido nesta verificação; conforto do mouse, diferentes DPIs, leitura por participantes e mixagem ainda exigem avaliação humana. A transcrição textual permanece disponível independentemente da audição.

## Prévia das mesas conferida na janela

Em 20/10/2026, a ferramenta Sky autorizada observou a primeira sala em `--preview-tables`: mesas do diário e da manutenção com madeira e apoio no piso. O modo é uma fixture estática com perfil isolado, e Esc encerrou normalmente. Não foi um percurso humano nem prova de movimentação/interação. `logs/mesas_cinco_preview_verificacao.json` confirmou modelo aceito, cinco instâncias e hashes de progresso/opções inalterados.

A abertura normal por `Jogar.cmd` também foi conferida na janela Release: o menu inicial normal apareceu e `logs/mesas_cinco_jogar.log` confirmou modelo aceito e cinco instâncias. A prévia estática anterior foi encerrada por Esc; o perfil havia sido verificado antes dessa abertura normal. Isso confirma o carregamento pelo iniciador do projeto, sem afirmar um percurso humano jogado.

## Distribuição e desempenho

O pacote `dist/O-Ultimo-Sinal-Investigacao-v3.zip` foi atualizado e revalidado para **cinco instâncias** em 20/10/2026. A execução do pacote passou em **58 verificações sem falhas**, carregou/desenhou as cinco cenas com GL0 e confirmou `table_instances=5`. O teste negativo de recurso obrigatório retornou código 2. Logs: `mesa_package_runtime.log` e `mesa_package_missing_resource.log`; resumo: `logs/mesa_package_verificacao.json`.

O ZIP possui **50.036.012 bytes**, SHA-256 `a932aba1a9264b6419a42ae86395240919eee4d2af1e7ba720d480336a182229`. Inclui o mesmo modelo/textura reutilizado em cinco instâncias. Evidências de produção permanecem nos fontes, fora do pacote de execução, e a distribuição anterior foi preservada em `dist/backups`. O teste em outro computador continua pendente.

O teste curto do benchmark confirmou FBO completo, resolução MSAA4 e conteúdo não uniforme. A coleta integral anterior à mesa, em Média/RTX 3050, registrou mediana de 3,0136 ms e p95 de 6,6885 ms em 1280 × 720. Esse resultado pertence àquela compilação; esta rodada não inclui novo benchmark. O método serializa CPU/GPU sem medir VSync, apresentação, latência ou experiência humana. Consulte `DESEMPENHO.md`.

Os registros desta rodada, resumo de hashes, logs de QA, quadros 09/10 e a captura nativa `mesas_primeira_sala.jpg` foram preservados em `docs/evidencias_2026-10-20`. Os registros da rodada anterior permanecem em `docs/evidencias_2026-10-19`.

## Histórico resumido

A versão anterior possuía 5.068 verificações puras e 27 verificações de percurso. Esses números pertencem à implementação antiga e não devem ser somados aos atuais. Os logs antigos foram preservados para rastreabilidade; não representam testes de enigmas, salvamento e apresentação adicionados agora.

## Pendências de aceite

- Percurso humano completo, com descobertas em ordens diferentes.
- Percorrer manualmente todas as telas de enigmas/relatório e transições restantes; menus iniciais, opções e parte dos controles já foram conferidos pela interface.
- Testar o comando remapeado interagindo com alvo no mundo, diferentes teclados/DPIs, retorno de foco e redução de movimento.
- Audição e avaliação da mixagem; funcionamento sem dispositivo de áudio.
- Comparação isolada das técnicas gráficas e inspeção de transparências/sombras em movimento.
- Avaliar desempenho na janela visível e no computador de apresentação; a coleta interna acima não substitui esses testes.
- Refinamento dos modelos e avaliação visual do bake estático do piso; produção, reabertura e UVs já conferidos nesta rodada.
- Piloto com cinco participantes, registrando respostas antes do feedback.

Nenhum tempo humano, resultado de compreensão ou participante foi inventado. O programa não avalia semanticamente a justificativa e os testes técnicos não garantem nota acadêmica.

A inspeção pela janela também identificou faces coplanares na fundação, no piso e no forro. A fundação foi rebaixada e os ressaltos separados 5 cm das paredes em ambos os eixos; a sétima imagem e a sala interna foram revisadas após a correção. Colisões e percurso permanecem preservados. O registro `ui_checks_2026-10-19.json` identifica a compilação usada nos testes de menus, anterior à correção geométrica e à integração da mesa.
