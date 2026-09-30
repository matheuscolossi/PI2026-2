# Planejamento Matheus

> **Atualização de 29/09/2026:** Matheus autorizou iniciar a produção do zero nesta pasta. O texto abaixo preserva o diagnóstico original; seu estado de espera por aprovação foi superado por essa instrução. Consulte `STATUS.md` para o progresso atual. Nenhuma tarefa futura deve ser considerada concluída sem evidência.

## O Último Sinal — plano do estado atual à conclusão

**Data do diagnóstico:** 29/09/2026  
**Integrantes identificados nas entregas:** Matheus Lopes Colossi e Gabriel Novelo Javornik  
**Disciplinas:** Projeto Integrador VI e Computação Gráfica  
**Situação:** planejamento para revisão; implementação ainda não autorizada.

> Este documento organiza o trabalho. As tarefas propostas só começam após aprovação explícita do grupo. O plano não comprova execução de tarefas futuras.

## 1. Resumo do projeto

O Último Sinal é um jogo investigativo 3D para computador, ambientado em uma pequena ilha com um farol e duas salas. O jogador explora o cenário, examina cinco pistas e escolhe uma conclusão fundamentada nas evidências. A duração pretendida é de 5 a 10 minutos.

O escopo exclui combate, inimigos, NPCs, mundo aberto e inventário complexo. Não há necessidade prevista de contas, servidor, banco de dados ou APIs externas.

As cinco pistas definidas são:

1. Diário do faroleiro.
2. Ferramenta quebrada.
3. Pegadas molhadas.
4. Chave da sala da lente.
5. Fotografia rasgada.

O projeto precisa atender simultaneamente a duas frentes:

- **Projeto Integrador:** investigação coerente, metodologia aplicada, avaliação com jogadores e relatório com resultados e limitações.
- **Computação Gráfica:** demonstração das técnicas exigidas, usando a base disponibilizada pelo professor e acompanhando sua evolução nas aulas.

## 2. Como este diagnóstico foi fundamentado

Foram consultados o contexto da conversa, o código local, o histórico Git, as configurações do Visual Studio, o registro de compilação, o README, as duas entregas escritas e materiais atuais no Google Drive.

Não houve nova compilação nem novo teste de execução durante o diagnóstico de 29/09. As evidências de execução são da sessão de 15/09. A criação deste documento não inicia a implementação.

### Evidências confirmadas

| Evidência | Constatação |
|---|---|
| Histórico Git | Dois commits: base original e primeiro protótipo. |
| Base local | Versão de 14/09/2026, preservada em arquivo e referência Git. |
| Base no Drive | Outro arquivo BaseAppOGL.rar, modificado em 22/09/2026 às 00:57 UTC. Seu conteúdo ainda não foi comparado com o local. |
| Compilação anterior | Registro de sucesso em Debug x64 em 15/09. |
| Execução anterior | Cenário renderizado e abertura do caderno conferida. |
| Repositório remoto | Nenhum remoto configurado neste repositório local. |
| Arte do grupo | Não foram encontrados arquivos .max ou .3ds na pasta da cena do grupo. Isso não exclui trabalho existente em outro local. |
| Entregas 1 e 2 | Arquivos confirmados; nota máxima informada por Matheus. Essa avaliação não valida o executável. |

### Vocabulário de acompanhamento

- **Planejado:** definido no documento, ainda sem implementação comprovada.
- **Em desenvolvimento:** tarefa iniciada e ainda incompleta.
- **Implementado:** comportamento presente no código.
- **Testado:** comportamento específico executado e observado.
- **Validado:** critério de aceite satisfeito com evidência.
- **Pendente:** falta desenvolver, decidir ou verificar.

## 3. Progresso atual

**Estágio: fundação, com protótipo inicial executável e parte das interações implementada.** Não há informação suficiente para atribuir uma porcentagem confiável.

| Parte | Estado real |
|---|---|
| Tema, recorte e cinco pistas | Definidos nos documentos. |
| Base e SDK | Configurados e testados anteriormente em Debug x64. |
| Organização do código próprio | Implementada em Scenes/UltimoSinal, com integração pequena no gerenciador de cenas. |
| Ilha, farol e duas salas | Implementados com formas provisórias; visualização inicial testada. |
| Movimento e colisões | Implementados; percurso completo não validado. |
| Mira e exame de pistas | Implementados; alcance, obstáculos e repetição ainda precisam de verificação integral. |
| Chave e passagem para a segunda sala | Implementadas; teste completo pendente. |
| Caderno | Abertura testada. Lista nomes e marcações, mas não recupera os textos completos das pistas. |
| Pausa, reinício e troca de cenas | Implementados; combinações e regressões pendentes de teste. |
| Mistério, alternativas e gabarito | Pendentes; textos atuais provisórios. |
| Encerramento | Parcial: cinco pistas geram uma mensagem, sem escolha de conclusão nem explicação. |
| Modelos e materiais finais | Pendentes na cena do grupo. |
| Técnicas gráficas | Pendentes de integração e demonstração no jogo. Classes existentes na base não comprovam aplicação na cena. |
| Desempenho e piloto | Planejados; resultados não confirmados. |
| Distribuição | Pendente; Release ainda não preparado. |

### O que já está pronto

Há uma fundação local que compilou e abriu, a estrutura inicial da cena, preservação da base original e documentos acadêmicos iniciais. Não há evidência suficiente para considerar o percurso completo validado.

### O que falta

Comparar e integrar a base atual, fechar a narrativa, validar as interações, implementar conclusão e feedback, produzir a arte, integrar as técnicas gráficas, preparar distribuição, testar, realizar o piloto e finalizar as entregas.

## 4. Problemas conhecidos, dívidas e riscos

| Ponto | Evidência ou risco | Consequência |
|---|---|---|
| Base desatualizada | Há arquivo mais novo na pasta 2024T2. | Desenvolvimento pode divergir das aulas e gerar retrabalho. |
| Narrativa indefinida | Pistas não explicam ainda um mistério resolvível. | Bloqueia textos finais, conclusão, parte da arte e piloto. |
| Release divergente | Configuração usa caminhos antigos, C++17 e dependências diferentes do Debug. | Pode funcionar apenas no ambiente de desenvolvimento. |
| Caminhos relativos | Shaders dependem da pasta de execução. | Executável aberto em outro contexto pode falhar. |
| Classe concentrada | Cenário, entradas, regras, interface e desenho estão juntos. | Integração dos efeitos aumenta dificuldade de manutenção. |
| Chave pelo índice 3 | Regra depende da ordem das pistas. | Reordenação pode alterar o comportamento silenciosamente. |
| Estados sobrepostos | Pausa e caderno são booleanos independentes. | Combinações precisam de regras claras e testes. |
| Visibilidade aproximada | Segmento da mira é amostrado em 31 pontos. | Obstáculos finos podem não ser detectados. |
| Tempo inadequado ao piloto | elapsed exclui leitura/pausa e limita o tempo somado por quadro. | Não representa a duração da sessão prevista na metodologia. |
| Importador pressupõe atributos | Código local acessa normais e tangentes sem verificar sua presença. | Modelos incompatíveis podem falhar; problema ainda não reproduzido no protótipo. |
| Testes limitados | Não há percurso integral comprovado. | Acessibilidade das pistas e conclusão permanecem incertas. |
| Git somente local | Nenhum remoto configurado. | Risco para colaboração, cópia de segurança e entrega no forge. |

Na execução anterior, uma captura após abrir o caderno deixou de mostrar a cena. A causa não foi estabelecida: câmera, foco ou captura são hipóteses. Reproduzir antes de decidir uma correção.

### Dependências externas

Base e SDK do professor; Visual Studio e toolset compatíveis; computador com OpenGL 4.6; acesso ao 3DS Max; produção dos modelos; disponibilidade dos participantes; acesso ao forge e orientações das disciplinas.

## 5. Estado final desejado

### Fluxo principal

**Iniciar → explorar → examinar e consultar pistas → obter a chave → acessar a segunda sala → escolher uma conclusão → registrar justificativa antes do feedback → receber explicação → reiniciar ou sair.**

Proposta pendente de aprovação: exigir as cinco pistas antes de liberar a conclusão.

| Área | Resultado esperado |
|---|---|
| Jogabilidade | Percurso completo, controles claros, pistas acessíveis e estados consistentes. |
| Narrativa | Gabarito fixado, alternativas plausíveis e explicação apoiada nas evidências. |
| Cenário | Ilha, farol, duas salas e objetos finais, mantendo o recorte curto. |
| Gráficos | Cada técnica obrigatória demonstrada e associada ao código e à produção dos recursos. |
| Arquitetura | Base preservada; regras, conteúdo, interface e desenho organizados no código do grupo. |
| Integrações | Classes do professor e caminho de importação ensinado em aula. |
| Banco, APIs, autenticação | Não necessários para o jogo local previsto. |
| Erros | Falhas de recurso e inicialização identificáveis, com diagnóstico compreensível. |
| Segurança | Recursos locais conhecidos; sem credenciais no projeto; execução sem administrador; piloto sem identificação nominal nos resultados. |
| Logs | Versão, configuração gráfica, erros relevantes e medições técnicas; sem telemetria externa. |
| Testes | Regras de investigação, percurso, regressões, efeitos e execução fora da pasta de desenvolvimento. |
| Distribuição | Pacote Windows com executável, bibliotecas e recursos; fonte entregue conforme orientação. |
| Documentação | Compilação, execução, controles, requisitos, evidências, resultados, limitações e créditos. |

### Técnicas de Computação Gráfica

| Técnica | Aplicação planejada | Evidência necessária |
|---|---|---|
| Modelagem .3DS e UVW | Farol, móveis e pistas | Arquivos do 3DS Max, exportação e textura no jogo. |
| Point e Spot | Lâmpadas e iluminação do farol | Comparação com luz ativa/inativa. |
| Multitextura | Areia, grama e rocha | Composição visível e texturas identificadas. |
| Skybox | Céu da ilha | Continuidade durante rotação. |
| Environment mapping | Água e metal | Resposta aparente à direção de observação. |
| Reflexão e refração | Água e lente de vidro | Demonstração separada dos dois efeitos. |
| Normal mapping | Pedra, madeira e paredes | Comparação do material com/sem mapa. |
| Fog | Neblina | Efeito de distância demonstrado. |
| Blending | Água e janelas | Transparência e ordenação corretas. |
| Bake | Sombras dos objetos fixos | Textura produzida no 3DS Max e aplicada na cena. |

A rubrica admite bump ou normal mapping e descreve iluminação com fonte Point/Spot. O documento do grupo assume normal mapping e Point e Spot; manter esse compromisso até eventual ajuste discutido. Transparência, isoladamente, não comprova refração. Céu de fundo, isoladamente, não comprova environment mapping nos materiais.

### Critérios objetivos de conclusão

1. Nenhuma falha bloqueante no percurso completo e no reinício.
2. Todas as pistas, alternativas e feedback funcionando conforme o gabarito.
3. Todas as técnicas obrigatórias com evidências verificáveis.
4. Pacote executado no computador de apresentação.
5. Piloto realizado conforme o procedimento acordado, com resultados reais e limitações.
6. Fonte, documentação e entregas finais organizados.

As metas propostas nos documentos são: 4 de 5 participantes acertarem com justificativa de duas pistas; 4 de 5 concluírem sem ajuda narrativa; mediana de duração entre 5 e 10 minutos; mediana de clareza ≥ 4; e p95 de tempo de quadro ≤ 33,3 ms em 1280 × 720. São metas do grupo, não resultados nem garantia de nota. Resultados abaixo delas devem ser relatados e orientar ajustes.

## 6. Análise de lacunas

| Lacuna | Por que é necessária | Dependências | Complexidade | Risco e impacto |
|---|---|---|---|---|
| Atualização da base | Seguir as aulas | Nova base e referência antiga | Média, incerta antes da comparação | Afeta todos os componentes. |
| Narrativa e gabarito | Tornar a investigação resolvível | Decisão do grupo | Média | Mudanças tardias refazem textos, arte e testes. |
| Navegação e interação | Garantir acesso às pistas | Base compatível | Média | Pode exigir alterar o mapa. |
| Caderno e conclusão | Fechar o ciclo da partida | Narrativa e estados | Média | Bloqueia o piloto. |
| Exportação .3DS | Garantir entrada da arte no jogo | 3DS Max e importador | Média | Produzir toda a arte antes do teste é arriscado. |
| Cenário e objetos | Atender modelagem e visual | Mapa e exportação aprovados | Alta | Pode consumir mais tempo que programação. |
| Técnicas gráficas | Atender CG | Aulas, modelos, UVs e materiais | Alta | Interações entre efeitos, desempenho e legibilidade. |
| Erros e distribuição | Executar fora do ambiente atual | Dependências compatíveis | Média | Bibliotecas ausentes podem impedir apresentação. |
| Testes e piloto | Sustentar resultados do PI | Versão estável | Média | Coleta prematura registra defeitos evitáveis. |
| Texto e entrega | Concluir o trabalho acadêmico | Evidências e resultados | Média | Não deve ficar todo para o fim. |

## 7. Decisões técnicas propostas

| Decisão | Opção A | Opção B | Recomendação |
|---|---|---|---|
| Atualizar a base | Copiar manualmente: rápido, mas aumenta risco de sobrescrita. | Referência Git e comparação antes da integração: exige disciplina, preserva histórico. | B; registrar aula, arquivo original e testes das cenas 0/1/2. |
| Organizar o jogo | Manter tudo em UltimoSinal: simples inicialmente, acoplamento crescente. | Separar estado, conteúdo, interface e desenho gradualmente: facilita manutenção e testes. | B, sem reescrever a engine. |
| Dados das pistas | Estruturas C++ e IDs nomeados: simples, exige recompilar para editar. | JSON externo: facilita conteúdo, adiciona leitura e validação. | A para cinco pistas e uma fase. |
| Colisões | Volumes simples e interseção geométrica: baixo custo, formas aproximadas. | Motor de física: mais recursos, maior integração. | A; PhysX não é necessário ao recorte. |
| Interface | Adaptar ImGui existente: menor custo, exige cuidado visual. | Interface própria em OpenGL: mais controle, mais trabalho. | A. |
| Avaliação | Formulário do observador: simples, exige padronizar coleta. | Formulário no jogo: automatiza, aumenta implementação e testes. | A, com resposta e justificativa antes do feedback. |
| Persistência | Sessão em memória: simples, fechamento perde partida. | Salvamento: permite retorno, exige compatibilidade e testes. | A para sessões curtas. |

### Arquitetura esperada

Manter CSceneManager → UltimoSinal, reutilizando câmera, shaders, entrada e recursos da base. Organizar o código próprio em responsabilidades pequenas: estado da investigação, conteúdo das pistas, interface e apresentação da cena. As regras devem ser testáveis sem depender da abertura da janela OpenGL.

Não trocar de engine nem criar uma arquitetura genérica de jogos. Mudanças na base devem ser pequenas, justificadas e rastreáveis, evitando conflito com as aulas seguintes.

## 8. Roadmap até a conclusão

| Fase | Objetivo e ordem | Dependências | Conclusão da fase | Risco principal |
|---|---|---|---|---|
| F0 — Requisitos e base | Confirmar marcos, comparar versões e integrar atualização. | Aprovação do plano | Base atual registrada, compilação e cenas verificadas. | Incompatibilidades entre aulas. |
| F1 — Narrativa e regras | Gabarito, textos, alternativas, circulação e estados. | Requisitos; pode avançar durante análise da base | Investigação completa no papel e revisada. | Ambiguidade do mistério. |
| F2 — Percurso com blocos | Controles, pistas, caderno, porta, conclusão e reinício. | Base integrada e narrativa | Partida completa funciona com geometria provisória. | Falhas de interação não detectadas. |
| F3 — Arte e gráficos | Exportação de teste, modelos e técnicas das aulas. | Base; mapa estável para arte final | Cena final com evidências de cada técnica. | Compatibilidade de exportação e materiais. |
| F4 — Confiabilidade | Erros, Release, pacote, desempenho e regressões. | Começa cedo; fecha depois de F2/F3 | Pacote reproduzível e testes técnicos aprovados. | Funcionar só no computador atual. |
| F5 — Piloto | Instrumentos, sessões, análise e ajustes. | Versão estável e gabarito fixado | Dados reais consolidados e correções verificadas. | Misturar resultados de versões diferentes. |
| F6 — Entrega | Fonte, relatório, evidências, pacote e ensaio. | Resultados; documentação começa antes | Entregas conferidas e demonstração ensaiada. | Prazo acadêmico anterior ao presumido. |

## 9. Tarefas executáveis

**Prioridades:** P0 = bloqueador; P1 = necessário para a versão final; P2 = melhoria importante sem bloquear a primeira versão completa; P3 = opcional.

**Estado de todas as tarefas abaixo:** propostas, pendentes de aprovação e execução. Componentes novos são propostas, não arquivos já criados.

### F0 — Requisitos e base

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F0.1 · P0 | Consolidar requisitos, datas, responsáveis e horas para planejar contra prazos reais. | Grupo, cronogramas e entregas | Obrigações com data, responsável e evidência; revisão conjunta. |
| F0.2 · P0 | Comparar base atual com 14/09 para identificar conflitos. | Aprovação; arquivos e referência Git | Diferenças e estratégia de integração revisadas, preservando código próprio. |
| F0.3 · P0 | Integrar atualização e registrar nova referência funcional. | F0.2; engine, projeto, SDK e gerenciador | Debug x64 compila; cenas 0, 1 e 2 executam; resultados registrados. |

### F1 — Narrativa e regras

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F1.1 · P0 | Escrever acontecimento, pistas, alternativas e gabarito para sustentar a investigação. | F0.1; roteiro e conteúdo | Matriz explica resposta e alternativas; os dois autores revisam ambiguidades. |
| F1.2 · P1 | Fixar mapa, objetos e função da chave para evitar refazer arte. | F1.1; cenário e colisões | Percurso alcança todas as pistas; verificar acessos antes/depois da chave. |
| F1.3 · P1 | Definir exploração, leitura, caderno, pausa e conclusão. | F1.1; estado e interface | Cada comando tem efeito definido em cada estado; revisar sequências de transição. |

### F2 — Investigação funcional

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F2.1 · P1 | Separar regras e usar IDs estáveis para evitar dependência de índices. | F0.3, F1.3; UltimoSinal e estado | Coleta repetida não duplica; chave e reinício independem da ordem; testes de regras. |
| F2.2 · P1 | Validar movimento, mira, colisões, foco e cursor. | F1.2, F2.1; câmera, entrada e obstáculos | Percurso acessível sem atravessar/examinar através de paredes; testar cantos, menus e foco. |
| F2.3 · P1 | Completar leitura e caderno para comparar evidências. | F1.1, F2.1; interface e dados | Textos descobertos podem ser relidos; marcações corretas em repetição/reinício. |
| F2.4 · P1 | Implementar conclusão e feedback para fechar a partida. | F1.1, F1.3, F2.3; conclusão e interface | Todas as alternativas explicadas; resposta e justificativa registráveis antes do feedback. |
| F2.5 · P1 | Testar a partida integral antes da arte final. | F2.2–F2.4; cena | Ordens diferentes, porta e reinício aprovados; falhas registradas e correções retestadas. |

### F3 — Arte e gráficos

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F3.1 · P0 | Provar 3DS Max → .3DS → jogo antes de produção em volume. | F0.3 e 3DS Max; modelo e CAsset | Objeto com UV, textura e atributos aparece na escala/orientação corretas; comparar etapas. |
| F3.2 · P1 | Produzir cenário e objetos finais. | F1.2, F3.1; modelos, texturas e cena | Fontes preservadas, pistas reconhecíveis, circulação mantida; inspeção e percurso. |
| F3.3 · P1 | Integrar Point/Spot e normal mapping conforme aulas. | F3.1 e referência das aulas; shaders e materiais | Comparações ativa/inativa demonstram técnicas sem ocultar pistas. |
| F3.4 · P1 | Integrar multitextura e skybox. | F3.1; terreno, texturas e shaders | Materiais identificáveis e céu contínuo; capturas e rotação da câmera. |
| F3.5 · P1 | Demonstrar environment mapping, reflexão e refração. | Recursos da base e materiais; água, metal e lente | Evidências distintas; transparência sozinha não conta como refração. |
| F3.6 · P1 | Integrar fog e blending preservando navegação. | F3.4/F3.5; desenho e transparências | Distância afeta fog; transparências sem erros de ordenação no percurso. |
| F3.7 · P1 | Produzir e aplicar bake de sombras estáticas. | F3.2, iluminação e UVs estáveis; Max e materiais | Bake comprovado na origem e no jogo, sem iluminação duplicada indevida. |
| F3.8 · P1 | Consolidar evidências gráficas para avaliação. | F3.3–F3.7; capturas, código e modelos | Cada requisito aponta para implementação e evidência da mesma versão; conferir rubrica. |

### F4 — Confiabilidade e distribuição

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F4.1 · P1 | Tratar falhas de recursos e pressupostos do importador. | F0.3, F3.1; carregamento, inicialização e logs | Recursos ausentes/inválidos geram diagnóstico; testar cópias controladas com falhas. |
| F4.2 · P1 | Preparar Release x64 e pacote com caminhos previsíveis. | F0.3; vcxproj, bibliotecas e recursos | Pacote abre fora do desenvolvimento sem depender de DLLs Debug instaladas. |
| F4.3 · P1 | Medir desempenho conforme metodologia. | F3.8, F4.2; quadros e ambiente | Três coletas de 120 s após 30 s de aquecimento; configuração e p95 registrados. |
| F4.4 · P1 | Validar versão candidata no computador-alvo. | F2.5, F4.1–F4.3; pacote | Percurso, reinício, resolução e foco aprovados; nenhuma falha bloqueante aberta. |

### F5 — Avaliação

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F5.1 · P1 | Preparar formulário, instruções e tempo para executar a metodologia. | F1.1 e orientações institucionais; instrumentos | Ensaio dos autores confirma coleta antes do feedback e registro de interrupções. |
| F5.2 · P1 | Realizar piloto com participantes elegíveis. | F4.4, F5.1 e voluntários; versão identificada | Registros completos, incluindo desistências e incompletas; conferir consistência. |
| F5.3 · P1 | Analisar dados e corrigir problemas prioritários. | F5.2; relatório e componentes afetados | Contagens, tempos, clareza e dificuldades apresentados; reteste e distinção de versões. |

### F6 — Finalização

| ID / prioridade | Objetivo e motivo | Dependências / componentes | Aceite e validação |
|---|---|---|---|
| F6.1 · P1 | Confirmar destino e preparar entrega do fonte no forge. | F0.1 e acesso do grupo; repositório e documentação | Fonte reproduzível, sem arquivos pessoais/binários desnecessários; conferir acesso e prazo antes de publicar. |
| F6.2 · P1 | Finalizar resultados, conclusões, referências, créditos e instruções. | F3.8, F5.3; relatório e README | Texto corresponde ao implementado/medido; revisão dos dois autores. |
| F6.3 · P1 | Preparar pacote final, cópia de segurança e ensaio. | F4.4, F6.1/F6.2; versão e apresentação | Demonstração no tempo disponível; arquivos exigidos conferidos. |
| F6.4 · P2 | Refinar tipografia, mensagens e apresentação. | Fluxo e gráficos estáveis; interface | Legibilidade melhorada sem regressões nem atraso dos essenciais. |

**P3 — Fora do caminho principal:** áudio elaborado, partículas, sombras dinâmicas, animações adicionais, salvamento, conquistas, novas fases e versão móvel. Só entram com nova decisão de escopo após cumprir os essenciais.

## 10. Caminho crítico e divisão de trabalho

### Frente de investigação

Base atual → regras e narrativa → investigação completa → versão estável → piloto → resultados → entrega.

### Frente gráfica

Base atual → teste de exportação → modelos e materiais → técnicas gráficas → desempenho e pacote → entrega.

### O que vem primeiro

Confirmar marcos acadêmicos; comparar e integrar a base; fixar o gabarito; validar uma exportação antes de produzir toda a arte.

### O que pode avançar em paralelo

- Narrativa e comparação da base.
- Interação do protótipo e prova de exportação.
- Modelagem, após o mapa estável, e shaders em objetos de teste.
- Instrumentos do piloto e documentação durante o desenvolvimento.

Essa divisão é entre os integrantes; responsáveis ainda precisam ser definidos. Não representa autorização para execução automática de tarefas.

### O que pode esperar

Polimento, áudio e efeitos adicionais. Os maiores riscos de retrabalho são arte antes do mapa, feedback antes do gabarito, permanência prolongada em base antiga e piloto antes da estabilização.

## 11. Avaliação e operação

O piloto proposto usa cinco estudantes adultos, familiarizados com teclado e mouse, excluindo autores e pessoas que já conheçam o desfecho. A coleta depende das orientações institucionais e da disponibilidade efetiva dos participantes.

Usar a mesma versão e computador, instruções padronizadas e identificadores P01 a P05. Registrar conclusão e justificativa antes do feedback. Sessões acima de dez minutos continuam registradas; aos quinze minutos, o procedimento descrito na metodologia prevê encerrar e marcar a tentativa como incompleta.

Registrar tempo, pistas examinadas, alternativa, justificativa, ajuda, clareza de 1 a 5 e dificuldades. Informar desistências e falhas separadamente. Analisar contagens com denominadores, medianas, faixas e categorias de problemas. A amostra pequena não demonstra ganho geral de raciocínio nem eficácia educacional.

Para operação, priorizar pacote local reproduzível, logs técnicos, cópia de segurança e teste no equipamento real. Não há deploy em nuvem previsto.

## 12. Datas e decisões pendentes

### Datas verificadas e limitações

- **CG, turma 2024T2:** apresentação final indicada para **23/11/2026**.
- **PI, segunda etapa:** documento indica janela de 15/09 a 10/11 e marcos de roteiro, aplicativo, código no forge e entrega final. A extração textual não preservou a associação dos marcos às datas; os prazos específicos precisam de confirmação.
- Não planejar apenas contra 23/11: PI prevê marcos anteriores.
- Sem horas semanais e divisão de tarefas, não há estimativa confiável de duração de cada fase.

### Decisões que o grupo precisa tomar

1. Quais são as próximas datas de PI?
2. Quantas horas por semana cada integrante pode dedicar?
3. Há código ou modelos feitos em outra pasta?
4. Quem assume programação, modelagem, testes e texto?
5. Qual é o acontecimento central, o gabarito e quais são as alternativas?
6. Todas as cinco pistas serão exigidas para concluir?
7. Qual computador será usado como referência e na apresentação?
8. Qual é o forge exigido e qual o estado do cadastro do grupo?
9. Há acesso funcional ao 3DS Max e participantes disponíveis?

## 13. Próxima fase recomendada

**F0 — Requisitos e atualização da base.** Seguir a evolução das aulas é uma restrição central, e existe uma atualização concreta a comparar.

Após aprovação explícita, as primeiras cinco tarefas propostas são:

1. **F0.1:** fechar requisitos, marcos e divisão de trabalho.
2. **F0.2:** comparar a base atual com a versão de 14/09.
3. **F0.3:** integrar mudanças e verificar as três cenas.
4. **F1.1:** definir e revisar a matriz das pistas e do gabarito.
5. **F3.1:** provar exportação de um objeto do 3DS Max para o jogo.

## 14. Referências e arquivos para continuidade

### Materiais no Drive

- [Pasta 2024T2 — base do professor](https://drive.google.com/drive/folders/1TXdMUWoPl0R2pQ5_p_PZvNxFcrh8RFRN)
- [Base identificada em 29/09, modificada em 22/09 UTC](https://drive.google.com/file/d/1hW9BJaKuObhZD1-5-hCTysnZ4EDwzzoK/view)
- [Critérios do trabalho final de CG](https://drive.google.com/file/d/1ysIhN584aX6Ye_HJ-5MpSdSHo0grpPk3/view)
- [Cronograma da segunda etapa de PI](https://drive.google.com/file/d/1Yc79HzuKUkNV3jZ2krzSRulOOtUsgYsa/view)
- [Pasta de Projeto Integrador](https://drive.google.com/drive/folders/1oIy2MLkwUZFv468f50_MbQ62LOm2skqH)

### Localização do projeto neste computador

Pasta principal: `C:\Users\mlc17\Documents\Codex\2026-09-08\https-drive-google-com-drive-u`.

Dentro dela:

- `outputs/O_Ultimo_Sinal/BaseAppOGL/BaseAppOGL.sln`: solução do Visual Studio.
- `outputs/O_Ultimo_Sinal/BaseAppOGL/Scenes/UltimoSinal`: código próprio e shaders provisórios.
- `outputs/O_Ultimo_Sinal/BaseAppOGL/Engine/CSceneManager.cpp`: registro da cena 2.
- `outputs/O_Ultimo_Sinal/BaseAppOGL/Directory.Build.props`: caminho local do SDK.
- `outputs/O_Ultimo_Sinal/OpenGL_SDK`: dependências.
- `outputs/O_Ultimo_Sinal/BaseOriginal`: arquivo original preservado.
- `outputs/O_Ultimo_Sinal/LEIA-ME.md`: execução e verificação anterior.
- `outputs/Entrega_1_O_Ultimo_Sinal_Roteiro.docx`: primeira entrega.
- `outputs/Entrega_2_O_Ultimo_Sinal_Referencial_Metodologia.docx`: segunda entrega.

Histórico identificado: `a80d052` (base original) e `9d9ce87` (protótipo). Referência: `professor-2026-09-14`.

## 15. Regra para continuar em outra tarefa do Codex

Ler este planejamento, inspecionar o estado atual do projeto e confirmar se houve mudanças desde o diagnóstico. Não assumir que uma tarefa está concluída por estar descrita aqui. Atualizar os estados com evidências durante o desenvolvimento autorizado.

**Não implementar, atualizar a base, publicar, alterar código ou começar o roadmap apenas por receber este documento. A implementação depende de aprovação explícita de Matheus, como: “Plano aprovado. Pode implementar.”**
