## Revisão atual — 22/10/2026: cenário e percurso vertical

Escada em espiral com 36 degraus transitáveis, abertura real na cobertura, patamar e sala óptica acessível. Três áreas conectadas no mesmo cenário: sala de serviço, arquivo e sala óptica. Acesso ao armário liberado, móveis reposicionados e 14 exports do Max atualizados, incluindo rádio, escrivaninha, diário, cadeira e fotografia. Conjunto: 39 modelos do cenário sem fallback mais mesa.3ds (40 arquivos); quatro mesas e uma escrivaninha.

Release/Debug e executável instalado: 306 verificações por execução, zero falhas. Lógica: 3.446/1.632 estados; apresentação: 46; áudio PCM: 38; suporte espacial: 185. Há 38 quadros de revisão e cinco cenas sem erro GL. Apenas o piso recebeu novo CompleteMap. Madeira e fotografia geradas são declaradas em CREDITOS.md. Progresso real preservado.

Procedimento, apresentação, alterações no código, evidências e pendências: [Revisão do cenário](Revisao_cenario_22-10-2026.md). Os blocos datados abaixo são históricos; seus totais e posições antigas não descrevem a versão atual.

## Revisão atual — 21/10/2026: investigação e inspeção

Três enigmas conectados: energia, rádio e novo ensaio local do comando da lente. Foram integrados console físico, inspeção 3D manipulável de quatro evidências, caderno agrupado com horários conhecidos, volumes de interação e áudio por distância/direção. Rascunhos e confirmações do console são salvos; partidas antigas incompletas preservam descobertas e relatórios já concluídos permanecem imutáveis.

Release/Debug e executável instalado: 218 verificações, zero falhas; lógica: 3.446 verificações/1.632 estados; apresentação: 46; áudio PCM: 38. Há 33 quadros do renderizador, 37 modelos do cenário aceitos sem fallback mais a mesa e cinco cenas de aula sem erro GL. Prévia dos controles feita em perfil isolado, sem alterar o progresso real. Ainda há acabamento artístico e avaliação com jogadores pendentes.

Guia, explicação do código, validação e limites: [Revisão profunda](Revisao_profunda_21-10-2026.md). Os registros datados abaixo preservam etapas anteriores e seus totais antigos.

# Requisitos e evidências — 20/10/2026

## Conferência final do conjunto do Max em 20 de outubro

Release e Debug foram compilados e executados: 178 verificações em cada configuração, zero falhas, 37 recursos do cenário aceitos e zero fallback. A mesa original permanece reutilizada em cinco instâncias. As cenas 0, 1, 5, 6 e 2 desenharam sem erro OpenGL. Foram exportados 18 quadros de revisão.

O Master cenario.max foi salvo e reaberto no Max. A auditoria dos 37 grupos registrou 628 componentes, todos com Unwrap UVW; o template nativo piso_uv.png foi exportado e conferido. O piso usa CompleteMap Scanline 1024 x 1024 em UV1, sem relighting Point/Spot. As demais geometrias mantêm luzes e sombras analíticas. A comparação nativa de sombras alterou 13.922 pixels acima de 12 níveis.

O pacote jogável foi gerado e executado a partir de pasta de trabalho externa, com 178 verificações sem falhas. O teste negativo de recurso ausente retornou código 2. Os 38 modelos, seus bitmaps e o executável mantiveram os hashes entre origem, distribuição e ZIP. As versões anteriores foram preservadas em dist/backups. Fontes editáveis e evidências estão separadas em Cenario-3dsMax-Fontes.zip.

O teste curto de renderização teve 2 s de aquecimento e 3 s de coleta em 1280 x 720 com MSAA4: 1.115 quadros, mediana 2,4915 ms e p95 4,4612 ms na RTX 3050. É sanidade técnica; continuam pendentes a medição integral atual, a janela no computador da apresentação e a experiência com pessoas.

Pendências reais: polimento visual, percurso humano completo, controles em diferentes DPIs, técnicas e transparências em movimento, áudio, execução em outro computador e piloto com participantes. Não houve novo envio ao Git nem submissão acadêmica nesta rodada.

Evidências: logs/native_bake_runtime_release.log, logs/native_bake_runtime_debug.log, logs/native_bake_benchmark_smoke.log, logs/mesa_package_verificacao.json e Scenes/Models/Cenario/evidencias. Os totais históricos abaixo não devem ser somados aos atuais.

Pacote atual: 51,491,195 bytes; SHA-256 `4f41c8531e92a5668b51028fdb92922ce7b9a53178bf50cad6330dadfb5605e4`.

Referência de CG: [rubrica 2024T2](https://drive.google.com/file/d/1ysIhN584aX6Ye_HJ-5MpSdSHo0grpPk3/view). O planejamento do grupo e a melhoria de investigação não substituem os requisitos acadêmicos. Datas e submissões devem ser conferidas nos canais oficiais.

## Computação Gráfica na cena do grupo

| Requisito | Origem | Implementação atual | Estado e limite |
|---|---|---|---|
| Modelagem no 3ds Max / .3DS | Oficial | 37 exports do cenário e a mesa original; produção assistida por MAXScript nativo no Max 2027 | Produção e importação técnica confirmadas; Master final/UV e acabamento ainda em conferência |
| Unwrap UVW | Oficial | Auditoria nativa: 37 grupos/628 nós, zero nó sem Unwrap; UVs finitas em todos os modelos aceitos | Usa um canal UV exportável por `.3DS`; Master reaberto e template nativo do piso conferido |
| Point e Spot | Rubrica Point/Spot; grupo assume ambas | Luzes locais e foco junto à câmera; alimentação muda a apresentação | Implementados nos demais objetos; piso CompleteMap não recebe relighting Point/Spot |
| Multitextura | Oficial | Mistura de grama e pedra conforme caminho e litoral | Implementada; validar escala e transições nas posições de jogo |
| Skybox | Oficial | Seis faces com CSkybox da base | Implementado; conferir legibilidade/composição |
| Environment mapping | Oficial | Cubemap estático do céu amostrado por direção | Implementado; não captura objetos locais |
| Reflexão e refração | Oficial | `reflect`, `refract` e mistura por Fresnel | Implementadas sobre o cubemap; comparação visual isolada pendente |
| Bump ou normal | Oficial; grupo assume normal | Normal map de pedra com base tangente por derivadas | Implementado; comparar ligado/desligado |
| Fog | Oficial | Mistura por distância com cor do horizonte | Implementado; não substitui modelagem ou composição |
| Blending | Oficial | Água e vidro após os opacos, com mistura alfa | Implementado; conferir sobreposição e ângulos |
| Bake de sombras estáticas | Oficial | Piso: CompleteMap nativo Scanline/Render To Texture, 1024 × 1024, UV1; 13.922 pixels alterados na comparação sombras on/off | Produção comprovada somente no piso; outras superfícies mantêm sombras analíticas. Não equivale a todos os objetos baked |
| Som | Opcional da rubrica / melhoria solicitada | Síntese procedural estéreo de ambientes e equipamentos | Implementado, sem dublagem; validar mixagem com pessoas |

O piso usa `p_bake.jpg` no material 13, sem reaplicar luzes e sombras dinâmicas; exposição e neblina permanecem. Os demais objetos usam iluminação e sombras analíticas, que não são shadow maps nem bake. O conjunto importado utiliza UVs do Max; os mapas Noise convertidos em bitmaps têm autoria assistida declarada e não comprovam bake. A madeira é da aula, com procedência distinta da geometria/UV. Exemplos do professor não contam como produção do grupo. Evidência do piso: `Scenes/Models/Cenario/evidencias/bake_verificacao.json`.

## Investigação e experiência

| Item | Origem | Implementação / evidência | Limite |
|---|---|---|---|
| Ilha e duas salas | Planejamento | Mapa compacto, passagem com chave e colisões | Percurso manual completo pendente |
| Pergunta central | Melhoria solicitada | Sinal posterior à chamada da queda, verificado por documentos independentes | Compreensão deve ser avaliada no piloto |
| Cinco pistas clássicas | Planejamento | IDs preservados dentro de dez registros | Coletar as cinco não encerra automaticamente a progressão |
| Dois enigmas relacionados | Melhoria solicitada | Manutenção + ramal molhado; carta + sintonia do rádio | O código exige consulta das evidências correspondentes |
| Registros adicionais | Melhoria solicitada | Ficha, carta de navegação, transmissão, registro e carta pessoal | Carta pessoal opcional altera passagem do desfecho |
| Caderno e releitura | Planejamento / melhoria | Mostra somente descobertas e pergunta pendente | Não corrige automaticamente a interpretação do jogador |
| Ajuda gradual | Melhoria solicitada | Orientação, indicação e solução por solicitação | Soluções não aparecem automaticamente no HUD |
| Hipótese e justificativa | Metodologia do grupo | Alternativa, dois registros e texto antes do feedback; relatório enviado fica imutável e consultável | Argumentação avaliada por pessoa, sem IA |
| Salvamento / continuação | Melhoria solicitada | Arquivo versionado, checksum, validação e substituição atômica; rascunho periódico e salvamento ao destruir a cena | Um slot local; sessão em memória é protegida após falha, mas depende de a aplicação permanecer aberta |
| Opções funcionais | Melhoria solicitada | Volumes, brilho, qualidade padrão Média, sensibilidade, inversão e redução de movimento | Conforto e desempenho da janela visível pendentes |
| Remapeamento | Melhoria solicitada | Seis ações por letras A–Z, sem duplicatas | Teclas especiais continuam reservadas |
| Alternativa ao som | Melhoria solicitada | Transcrição relível do rádio e respostas textuais | Não há atuação de voz |
| Duração de 5–10 minutos | Meta do grupo | Tempo inclui leitura e pausas | Não confirmada por participantes |
| Cinco adultos / acerto / clareza | Protocolo do grupo | Protocolo permanece referência | Piloto pendente; nenhum resultado inventado |
| Fonte no forge | Entrega acadêmica | Repositório `matheuscolossi/PI2026-2` existente | Envio desta revisão e submissão acadêmica exigem confirmação própria |

## Engenharia e validação

- Estrutura C++/OpenGL mantida; cenas acadêmicas acessíveis por Ctrl+0/1/2/5/6.
- Cenário completo com bake compilado e verificado em Release e Debug, com 178 testes em cada configuração. A etapa anterior de cinco mesas foi compilada em ambas; saídas distintas `bin/Debug` e `bin/InvestigacaoRelease`.
- Solicitação de GPU NVIDIA por processo, sem alteração global do Windows; confirmar GPU utilizada no log.
- Arquivos locais em `%LOCALAPPDATA%/UltimoSinal`: `progress.v3` e `options.v1`.
- O progresso inclui posição, descobertas, mecanismos, tempo e relatório; dados inválidos são recusados.
- Testes puros desta revisão: **436 verificações**, incluindo **184 estados alcançáveis com caminho de conclusão**, oito circuitos e 16 canais.
- Verificação atual: **178 testes sem falhas**, 37 modelos do cenário aceitos, zero fallback nativo, CompleteMap do piso aplicado, cinco cenas GL0 e **18 quadros exportados**. Registro: `logs/native_bake_runtime_release.log`.
- Histórico preservado: etapa anterior sem bake teve 176 testes; cinco mesas tiveram 58 testes/dez quadros. O asset mesa continua com 60 triângulos e cinco instâncias; esse número não é uma exigência para os outros modelos.
- Dez testes novos protegem a imutabilidade da resposta após o feedback; o rascunho é salvo periodicamente mesmo durante digitação.
- Interface parcial do jogo verificada via Sky após autorização; prévia estática das mesas da sala inicial observada, com hashes de progresso/opções preservados. Mesa fonte conferida no Max via MAXScript assistido. Nenhuma dessas verificações substitui o percurso humano completo.
- O p95 histórico próximo de 7 ms pertence à versão anterior. Repetir a medição com o cenário revisado.
- Produção nativa do conjunto e bake local do piso comprovados; faltam conferência/salvamento final do Master/UV, acabamento, comparação visual dos efeitos, computador de apresentação e piloto. O smoke de 2 s + 3 s não substitui a coleta integral de desempenho.
- O ZIP disponível ainda corresponde às cinco mesas/58 verificações; o novo pacote com 38 modelos e bake e o ZIP de fontes aguardam a conferência final. O registro anterior `logs/mesa_package_verificacao.json` não certifica a distribuição desta nova etapa.

Consulte `STATUS.md` para a ordem das próximas etapas e `VALIDACAO.md` para resultados da execução integrada. Não há porcentagem de conclusão nem garantia de nota acadêmica.
