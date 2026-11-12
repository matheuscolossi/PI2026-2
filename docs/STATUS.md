## Revisão em conferência — 12/11/2026: gráficos, jogabilidade e momentos

Fontes alterados; **compilação no Visual Studio e execução no Windows pendentes**. Pós-processamento HDR opcional (SSAO e bloom), clima de 05h40, movimento com aceleração, corrida, escada suavizada, clique para examinar, painel de técnicas (F3) e momentos de apresentação em `Drama.h`. Em build de conferência para Linux com renderização por software: 306 verificações de runtime sem falhas, 52 quadros sem erro OpenGL, 3.446 de lógica/1.632 estados, 46 de apresentação, 46 de áudio PCM, 185 espaciais, 24 de atmosfera, 46 de momentos e 18 de percurso simulado. Esses números não certificam o MSVC, GPU real, desempenho, áudio audível nem partida humana. Os executáveis em `bin` e o pacote em `dist` continuam sendo os de 08/11; o executável em `bin` já lê os shaders novos da pasta `Scenes`.

Detalhes, limites, procedimento no Windows e retorno: [Melhorias de 12/11](Melhorias_12-11-2026.md).

## Revisão compilada mais recente — 08/11/2026: acabamento artístico e emocional

Primeira passagem de luz, cor, desgaste, chuva e acústica nas três áreas. Reflexões após descobertas aguardam o fechamento da leitura e não alteram enigmas. Continuar não repete descobertas conhecidas. O áudio reduz apenas o ambiente durante esses momentos; pulsos e efeitos de interação são preservados. Redução de movimentos remove a chuva em queda.

Release/Debug e executável instalado: 306 verificações por execução, sem falhas. Testes: 3.446 de lógica/1.632 estados, 46 de apresentação, 44 de áudio PCM, 185 espaciais e 24 de atmosfera. Conferência: 45 quadros, 39 imports do cenário sem fallback mais a mesa. Os modelos/masters do Max e o bake do piso não foram refeitos nesta etapa. A validação emocional com pessoas e a audição real permanecem pendentes.

Detalhes, procedimentos e imagens: [Acabamento artístico e emocional](Acabamento_artistico_08-11-2026.md). Os registros datados abaixo são históricos.

## Revisão atual — 22/10/2026: cenário e percurso vertical

Escada em espiral com 36 degraus transitáveis, abertura real na cobertura, patamar e sala óptica acessível. Três áreas conectadas no mesmo cenário: sala de serviço, arquivo e sala óptica. Acesso ao armário liberado, móveis reposicionados e 14 exports do Max atualizados, incluindo rádio, escrivaninha, diário, cadeira e fotografia. Conjunto: 39 modelos do cenário sem fallback mais mesa.3ds (40 arquivos); quatro mesas e uma escrivaninha.

Release/Debug e executável instalado: 306 verificações por execução, zero falhas. Lógica: 3.446/1.632 estados; apresentação: 46; áudio PCM: 38; suporte espacial: 185. Há 38 quadros de revisão e cinco cenas sem erro GL. Apenas o piso recebeu novo CompleteMap. Madeira e fotografia geradas são declaradas em CREDITOS.md. Progresso real preservado.

Procedimento, apresentação, alterações no código, evidências e pendências: [Revisão do cenário](Revisao_cenario_22-10-2026.md). Os blocos datados abaixo são históricos; seus totais e posições antigas não descrevem a versão atual.

## Revisão atual — 21/10/2026: investigação e inspeção

Três enigmas conectados: energia, rádio e novo ensaio local do comando da lente. Foram integrados console físico, inspeção 3D manipulável de quatro evidências, caderno agrupado com horários conhecidos, volumes de interação e áudio por distância/direção. Rascunhos e confirmações do console são salvos; partidas antigas incompletas preservam descobertas e relatórios já concluídos permanecem imutáveis.

Release/Debug e executável instalado: 218 verificações, zero falhas; lógica: 3.446 verificações/1.632 estados; apresentação: 46; áudio PCM: 38. Há 33 quadros do renderizador, 37 modelos do cenário aceitos sem fallback mais a mesa e cinco cenas de aula sem erro GL. Prévia dos controles feita em perfil isolado, sem alterar o progresso real. Ainda há acabamento artístico e avaliação com jogadores pendentes.

Guia, explicação do código, validação e limites: [Revisão profunda](Revisao_profunda_21-10-2026.md). Os registros datados abaixo preservam etapas anteriores e seus totais antigos.

# Acompanhamento — 20/10/2026

## Conferência final do conjunto do Max em 20 de outubro

Release e Debug foram compilados e executados: 178 verificações em cada configuração, zero falhas, 37 recursos do cenário aceitos e zero fallback. A mesa original permanece reutilizada em cinco instâncias. As cenas 0, 1, 5, 6 e 2 desenharam sem erro OpenGL. Foram exportados 18 quadros de revisão.

O Master cenario.max foi salvo e reaberto no Max. A auditoria dos 37 grupos registrou 628 componentes, todos com Unwrap UVW; o template nativo piso_uv.png foi exportado e conferido. O piso usa CompleteMap Scanline 1024 x 1024 em UV1, sem relighting Point/Spot. As demais geometrias mantêm luzes e sombras analíticas. A comparação nativa de sombras alterou 13.922 pixels acima de 12 níveis.

O pacote jogável foi gerado e executado a partir de pasta de trabalho externa, com 178 verificações sem falhas. O teste negativo de recurso ausente retornou código 2. Os 38 modelos, seus bitmaps e o executável mantiveram os hashes entre origem, distribuição e ZIP. As versões anteriores foram preservadas em dist/backups. Fontes editáveis e evidências estão separadas em Cenario-3dsMax-Fontes.zip.

O teste curto de renderização teve 2 s de aquecimento e 3 s de coleta em 1280 x 720 com MSAA4: 1.115 quadros, mediana 2,4915 ms e p95 4,4612 ms na RTX 3050. É sanidade técnica; continuam pendentes a medição integral atual, a janela no computador da apresentação e a experiência com pessoas.

Pendências reais: polimento visual, percurso humano completo, controles em diferentes DPIs, técnicas e transparências em movimento, áudio, execução em outro computador e piloto com participantes. Não houve novo envio ao Git nem submissão acadêmica nesta rodada.

Evidências: logs/native_bake_runtime_release.log, logs/native_bake_runtime_debug.log, logs/native_bake_benchmark_smoke.log, logs/mesa_package_verificacao.json e Scenes/Models/Cenario/evidencias. Os totais históricos abaixo não devem ser somados aos atuais.

Pacote atual: 51,491,195 bytes; SHA-256 `4f41c8531e92a5668b51028fdb92922ce7b9a53178bf50cad6330dadfb5605e4`.

Esta revisão modifica a investigação e a apresentação dentro do projeto C++/OpenGL existente. Não representa a entrega acadêmica final.

## Implementação

| Frente | Estado atual | Limite |
|---|---|---|
| Base acadêmica | Cenas 0, 1, 5 e 6 preservadas; jogo na cena 2; trocas acadêmicas exigem Ctrl | Conferir também no computador de apresentação |
| Mistério | Cronologia de queda, transmissão e comando manual; relação com o Aurora e Helena | A identidade do executor não é demonstrada |
| Progressão | Enigma de alimentação e enigma de rádio, cruzando documentos e observação externa | Teste com pessoas ainda pendente |
| Registros | Dez entradas; pistas, documentos e objetos ambientais possuem modelos do Max; a mesa original atende cinco instâncias | Clareza visual e compreensão ainda dependem de teste humano |
| Caderno | Descobertas para releitura e pergunta pendente; sem lista antecipada de objetos | Avaliar clareza e leitura com participantes |
| Interface | Menus, opções, remapeamento/persistência e parte das transições conferidos na janela pela ferramenta Sky autorizada | Não equivale a percurso humano completo ou piloto |
| Salvamento | Progresso versionado, checksum, validação e substituição atômica; rascunho salvo a cada cerca de 5 s e ao destruir a cena | Um slot local; falha de gravação preserva a sessão apenas enquanto a aplicação permanece aberta |
| Opções | Três volumes, brilho, qualidade padrão Média, sensibilidade, inversão e redução de movimento | Conforto e desempenho da janela visível ainda precisam de avaliação |
| Controles | Seis ações A–Z; duplicidade recusada e remapeamento Q persistido; J/F1/Esc/Tab conferidos na janela | Interação Q em alvo real, caminhada manual, diferentes teclados e DPIs pendentes |
| Som | Síntese procedural estéreo, ambientes e efeitos; transcrição textual do rádio | Não há dublagem; mixagem requer avaliação humana |
| Cenário e luz | 37 modelos do cenário aceitos pelo CAsset, mais a mesa original reutilizada cinco vezes; geometria/UVs importadas | Cobertura nativa confirmou 37 aceitos e zero fallback na execução; acabamento visual permanece avaliável |
| Environment mapping | Reflexão/refração por cubemap estático do céu | Não reflete nem refrata dinamicamente objetos locais |
| Compilação | Release do conjunto completo com bake do piso compilado; saídas Debug e Release continuam distintas | Ambas as configurações verificadas com 178 testes; avaliação humana permanece pendente |
| Piloto | Pendente | Nenhum participante ou resultado de compreensão inventado |
| Max / UVW | Produção nativa assistida: 37 exports do cenário e mesa; auditoria registra 37 grupos, 628 nós e zero nó sem Unwrap | Master reaberto, UV exportado e fontes empacotadas; refinamento visual permanece pendente |
| Bake | Piso com CompleteMap Scanline/Render To Texture 1024 × 1024, UV1; comparação sombras ligadas/desligadas | Somente o piso recebeu bake; nele não há relighting Point/Spot. Outros objetos mantêm sombras analíticas |

## Verificações confirmadas nesta revisão

A rodada mais recente do cenário completo e bake do piso passou em **178 verificações sem falhas**, aceitou os **37 modelos do cenário**, confirmou **zero fallback nativo**, aplicou o CompleteMap do piso e exportou **18 quadros**. As cenas 0, 1, 5, 6 e 2 carregaram/desenharam com GL0. Evidência: `logs/native_bake_runtime_release.log`. A rodada imediatamente anterior, sem o bake, registrou 176 verificações; os totais pertencem às respectivas versões e não são somados.

O bake foi realizado no Max, não produzido pelo shader nem por conversão de Noise: a comparação nativa alterou 13.922 pixels acima de 12 níveis. `Scenes/Models/Cenario/evidencias/bake_verificacao.json` registra renderer, UV1, resolução e hashes de `piso.3ds`/`p_bake.jpg`. A produção assistida e suas limitações constam em `CREDITOS.md`.

Os testes puros passaram em **436 verificações**. A exploração das regras alcançou **184 estados**, todos com um caminho de conclusão. Também foram verificadas as oito configurações possíveis dos três ramais e os 16 canais do rádio.

A rodada anterior de cinco mesas passou em **58 testes sem falhas**, cinco cenas GL0 e **dez quadros exportados**, incluindo `09_mesas_sala_inicial.bmp` e `10_bancada_radio.bmp`. Os testes acrescentaram quantidade/posições das cinco instâncias, profundidade do rádio e coerência entre desenho e colisão. O asset permanece uma malha de 60 triângulos; não são cinco novos modelos autorais. Logs: `logs/mesas_cinco_runtime.log` e registros de compilação daquela rodada; detalhes em `VALIDACAO.md`.

A restrição inicial da ferramenta de janela foi resolvida após autorização do usuário, backups e correção do guard de salvamento. A verificação autorizada via Sky confirmou persistência de brilho, alteração/restauração de sensibilidade e qualidade, remapeamento com duplicidade recusada e persistência após reabrir, cancelamento de novo jogo, continuação, caderno, ajuda inicial e pausa/retorno. Os arquivos de opções terminaram iguais ao backup; o progresso permaneceu igual durante menus/reinícios antes de Continuar. Não houve percurso humano completo nem audição dos efeitos. Backups: `work/ui_backup_2026-10-19`; detalhes em `VALIDACAO.md`.

A prévia nativa autorizada `--preview-tables` mostrou diário e manutenção texturizados e apoiados na primeira sala e foi encerrada com Esc. Trata-se de enquadramento estático isolado; não houve novo percurso humano. Os hashes de `progress.v3` e `options.v1` ficaram intactos, conforme `logs/mesas_cinco_preview_verificacao.json`.

## Desempenho

A coleta integral anterior à integração da mesa, em Média, RTX 3050, alvo interno 1280 × 720 e MSAA4, registrou mediana de 3,0136 ms e p95 de 6,6885 ms. Ela atendeu a meta interna naquela versão. O cenário completo com bake passou apenas pelo smoke de 2 s de aquecimento + 3 s de amostra, com FBO/conteúdo válidos; isso não constitui coleta integral nem mede apresentação visível ou conforto. Log: `logs/native_bake_benchmark_smoke.log`. Repetir o protocolo integral e a avaliação na janela/PC da apresentação. Método histórico em `DESEMPENHO.md`.

O pacote disponível `dist/O-Ultimo-Sinal-Investigacao-v3.zip` ainda corresponde à etapa anterior de **cinco mesas**, 58 verificações e teste negativo com código 2: 50.036.012 bytes; SHA-256 `a932aba1a9264b6419a42ae86395240919eee4d2af1e7ba720d480336a182229`. Ele ainda não representa os 38 modelos e o bake atuais. Novo runtime e `Cenario-3dsMax-Fontes.zip` aguardam conferência final do Master/UV, Debug e atualização dos documentos de apresentação. O computador de apresentação continua pendente.

A conferência no Max foi feita por `verificar_mesa.ms` no MAXScript Listener, com o aplicativo em inglês e a mesa enquadrada. `conferencia_max.txt`, `checker.png`, `uv.png`, `uv_editor.jpg` e `modelo_editor.jpg` estão em `Scenes/Models/Mesa/evidencias`. A madeira foi restaurada antes do salvamento/reabertura; o `.3ds` não recebeu o checker. Hashes: `logs/mesa_producao_verificada.json`.

## Arquivos do usuário

- Progresso: `%LOCALAPPDATA%/UltimoSinal/progress.v3`.
- Opções: `%LOCALAPPDATA%/UltimoSinal/options.v1`.
- O carregamento verifica versão, checksum e invariantes; erros são informados.
- Novo jogo pede confirmação antes de substituir descobertas e relatório, inclusive quando existe apenas a sessão em memória.
- Continuar reutiliza a sessão ativa se a gravação falhar; isso não garante recuperação após fechar o aplicativo.
- O relatório enviado pode ser consultado e não pode ser reescrito depois do feedback.
- Opções permanecem independentes do progresso.
- O executável solicita a GPU NVIDIA por processo, sem alterar preferências globais do Windows; verificar a GPU real no log.

## Próximos critérios de aceite

1. Fazer um percurso manual do início ao relatório, incluindo exploração fora da ordem esperada, interação remapeada em alvo real e funcionamento sem som; menus iniciais e persistência já tiveram conferência parcial pela interface.
2. Refinar o acabamento do conjunto importado, preservando a fonte e a evidência UV já conferidas; preservar arquivos editáveis e evidências junto do ZIP de fontes.
3. Inspecionar o bake do piso e documentar seu escopo; os outros objetos continuam com sombras analíticas. Conferir separadamente as técnicas e transparências em diferentes posições.
4. Conferir técnicas gráficas isoladamente e medir desempenho novamente nas qualidades disponíveis.
5. Validar pacote completo no computador de apresentação.
6. Realizar o piloto com cinco adultos; registrar hipóteses e justificativas antes do feedback, sem fabricar respostas.

O código e a documentação existem no repositório `matheuscolossi/PI2026-2`. Atualização remota e submissão acadêmica devem ser verificadas separadamente. O planejamento e os documentos históricos continuam referências, não evidências automáticas de conclusão da versão atual.
