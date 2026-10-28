# O Último Sinal — revisão profunda da investigação

**Data:** 21 de outubro de 2026  
**Projeto:** jogo de investigação em primeira pessoa, desenvolvido em C++ e OpenGL sobre a base de aula.  
**Estado:** versão jogável em desenvolvimento, com investigação completa em testes automatizados e acabamento ainda em evolução.

Augusto, o faroleiro, foi encontrado morto. Mesmo depois da chamada de emergência, o farol voltou a emitir um sinal. O jogador chega à Ilha das Brumas para confrontar vestígios, documentos e registros de equipamento, descobrir o que pode ser sustentado por evidências e apresentar um relatório.

Este documento separa os recursos que já existiam das mudanças desta etapa. O guia de uso não apresenta a configuração correta dos enigmas nem a hipótese final.

## 1. Principais problemas encontrados

### A investigação ainda terminava cedo demais

O jogo já possuía enigmas de energia e rádio, documentos e uma conclusão, mas reunir as cinco pistas principais, ouvir a transmissão e ler o registro mecânico bastava para liberar o relatório. Faltava uma ação em que o jogador confrontasse os dados sobre o funcionamento da lente. A descoberta podia parecer uma lista de itens concluída.

O caderno apresentava uma sequência de botões pouco organizada. A mesma região de interface misturava vestígios materiais, registros técnicos e a carta pessoal de Augusto. O jogador precisava de apoio para localizar e comparar informações sem receber a resposta pronta.

### Examinar um objeto não permitia observá-lo de perto

As leituras preservavam as informações narrativas, mas diário, alicate, chave e fotografia não tinham uma vista manipulável. A seleção usava a mesma regra de alinhamento ao centro para todos os objetos, sem considerar suas áreas diferentes. Na bancada da segunda sala, dois alvos próximos também precisavam ser distinguidos com clareza.

### Som, animação e salvamento precisavam acompanhar as novas ações

Uma reconstituição precisava produzir uma resposta compreensível com som e imagem, sem parecer que o jogador estava criando outro sinal histórico. Era necessário impedir sobreposição de reproduções, respeitar pausa e perda de foco e salvar tanto uma tentativa incompleta quanto a confirmação do diagnóstico.

Também era necessário preservar partidas antigas. Acrescentar um requisito ao relatório não poderia apagar descobertas existentes ou invalidar relatórios já transmitidos.

### O acabamento visual continua sendo um trabalho maior

O cenário compacto e a tecnologia da base impõem limites. As melhorias anteriores já reduziram arestas duras em parte dos modelos e revisaram iluminação e apresentação, mas isso não equivale a substituir todo o acervo por arte final detalhada. Nesta etapa, a melhoria visual nova concentra-se na bancada funcional, na inspeção 3D e na resposta da reconstituição.

## 2. Mudanças efetivamente implementadas

### Recursos preservados da revisão anterior

Estes recursos já estavam integrados antes desta etapa e continuam disponíveis:

- Menu inicial sobre o cenário 3D, continuação, confirmação de nova partida e menu de pausa.
- Prólogo com chegada à ilha e opção de pular, quatro capítulos de investigação e epílogo. Os capítulos acompanham o progresso no mesmo cenário; não são quatro mapas novos.
- Enigmas de instalação elétrica e recepção de rádio, chave guardada no armário e acesso restrito à sala da lente.
- Armário e porta animados, água em movimento, luz do farol e respostas visuais de equipamentos.
- Modelos produzidos no 3ds Max, UVs, bitmaps e exportação `.3DS`. Parte do mobiliário e dos equipamentos recebeu acabamento de bordas e curvas.
- Piso reposicionado corretamente sobre a fundação e CompleteMap de 1024 × 1024 aplicado por UV1. O bake é do piso; não de todo o cenário.
- Opções funcionais de volume, sensibilidade, inversão vertical, brilho, qualidade, redução de movimentos e remapeamento dos controles principais.

### Novo enigma: reconstituição local do comando da lente

Foi acrescentado um console físico na bancada da sala da lente. Ele possui seleção do modo de operação e duração de três pulsos. O jogador precisa reunir e relacionar a comunicação recebida, o registro mecânico e a fotografia antes de conferir uma configuração.

O sistema recusa tentativas sem as informações necessárias e configurações que não explicam os registros. O relatório de uma investigação nova só fica disponível depois da reconstituição conferida, além dos requisitos anteriores.

**O console faz um ensaio local de bancada.** Ele não transmite uma nova mensagem para o mar, não reescreve o registro mecânico e não acrescenta um acontecimento à noite investigada. Após a confirmação, uma demonstração audiovisual local pode ser reproduzida novamente. Os controles confirmados permanecem preservados.

A demonstração tem indicação visual e sonora. A reprodução não pode ser empilhada repetidamente pelo botão e seu tempo é interrompido ao pausar. A informação necessária também aparece por escrito.

### Inspeção 3D opcional

As leituras do diário, alicate, chave e fotografia agora oferecem **Ver objeto**. A visualização utiliza o modelo 3D que faz parte do jogo, com rotação, inclinação, aproximação e restauração da vista. É possível arrastar com o mouse ou usar os controles da interface.

A leitura continua aparecendo primeiro, e a inspeção só ocupa espaço quando aberta. Fechar a inspeção ou voltar ao jogo não elimina a evidência. Não é obrigatório manipular a vista para ter acesso ao texto do documento.

### Caderno organizado para comparação

Os registros foram separados em três grupos:

- **Registros e comunicações:** diário, manutenção, navegação, transmissão e registro mecânico.
- **Vestígios e objetos:** alicate, marcas, chave e fotografia.
- **História pessoal:** carta opcional de Augusto.

Uma área de horários aparece somente depois que os registros correspondentes foram encontrados. O caderno mantém uma pergunta contextual e registra a confirmação do ensaio. Ele organiza fatos conhecidos, sem escolher a hipótese final pelo jogador.

A ajuda permanece gradual: uma orientação geral, uma indicação mais específica e, somente quando solicitada, a solução direta. O jogo não abre automaticamente a última etapa da ajuda.

### Mira, respostas e áudio

A seleção dos objetos considera uma área coerente com seu tamanho e a proximidade da mira. A linha de visão e as paredes continuam sendo respeitadas. Isso facilita examinar os objetos sem permitir interação através do cenário e distingue o comando da lente do documento colocado perto dele.

O áudio foi organizado em uma mistura executada separadamente da renderização. Passos, ambiente e equipamentos usam o tempo das amostras de som, reduzindo sua dependência da taxa de quadros. O rádio e o quadro elétrico consideram distância e posição em relação ao jogador. Há transição entre exterior e interior, efeitos de interação e controles de volume que também silenciam os efeitos correspondentes.

O som da reconstituição é um ensaio local. A recepção de rádio mantém uma resposta de sintonia própria, sem acrescentar outra mensagem codificada à história.

### Continuação e compatibilidade

O salvamento passou a guardar a configuração de tentativa do console e a confirmação da reconstituição, além das descobertas anteriores. Configurações incompatíveis com uma confirmação são rejeitadas na leitura do arquivo.

Partidas antigas incompletas continuam com as descobertas preservadas e passam a ter a nova reconstituição pendente. Relatórios concluídos em versões anteriores continuam concluídos: a compatibilidade não obriga o jogador a refazer ou alterar uma conclusão já transmitida.

O tempo de investigação considera exploração, leituras, caderno e mecanismos enquanto a janela está ativa. Pausa, opções e menu inicial não acrescentam tempo à investigação.

## 3. Como jogar e acessar as novas funções

### Início e controles padrão

Abra **Jogar.cmd** na pasta **O Último Sinal**. Use **Continuar investigação** para recuperar a partida ou **Nova investigação** para iniciar outra. Nova investigação pede confirmação antes de substituir o progresso.

| Ação | Controle padrão |
| --- | --- |
| Mover | W, A, S, D |
| Observar | Mouse |
| Examinar ou guardar uma leitura | E |
| Abrir ou fechar o caderno | J |
| Pausar ou voltar | Tab / Esc |
| Ajuda gradual | F1 |
| Opções | F2 |
| Pular o prólogo | Enter ou botão da introdução |
| Preparar relatório, quando disponível | Enter ou botão do caderno |

Os controles principais podem ser alterados em **Controles**. Se a câmera estiver desconfortável, ajuste a sensibilidade em **Opções** e experimente **Reduzir movimentos de câmera**.

### Percurso de investigação, sem entregar as soluções

1. Observe o acesso externo e a sala de serviço. Leia os documentos curtos e compare o procedimento de manutenção com o estado real dos equipamentos.
2. Resolva a alimentação auxiliar com base nessas informações. Observe o que mudou na sala e investigue os acessos liberados.
3. Consulte a navegação antes de usar o receptor. A escuta correta arquiva a comunicação no caderno.
4. Explore a sala da lente. Leia os registros e examine a fotografia antes de tentar explicar a emissão do sinal.
5. Examine o **Comando da lente**, na bancada da segunda sala. Também é possível abrir a bancada pelo botão **Examinar comando da lente** durante a leitura do registro mecânico.
6. Configure o ensaio a partir das evidências e use **Reconstituir sinal**. A interface informa se faltam registros ou se a configuração não corresponde a eles. Se precisar, peça ajuda gradualmente.
7. Depois da confirmação e das demais descobertas, prepare o relatório. Escolha uma hipótese, relacione pelo menos duas evidências e escreva uma justificativa antes de transmiti-lo.

Para usar a inspeção 3D, abra uma das quatro evidências compatíveis e escolha **Ver objeto**. Arraste o modelo ou use **Rotação**, **Inclinação** e **Aproximação**. **Restaurar vista** devolve o enquadramento inicial.

A carta pessoal aprofunda a história de Augusto e Helena, mas não é uma chave de progressão. O jogador pode concluir a investigação sem encontrá-la.

### Roteiro curto para apresentar o projeto

Mostre o menu e alguns segundos do prólogo; entre na exploração e demonstre a interação com um documento. Abra **Ver objeto**, gire o modelo e consulte o caderno para mostrar a organização dos registros. Apresente um mecanismo sem revelar sua configuração correta. Se houver uma partida preparada para demonstração, mostre a resposta local do console e explique que o relatório exige confrontar evidências.

Uma formulação útil para a apresentação é: “O jogador não avança apenas recolhendo objetos. Ele cruza informações do ambiente, dos documentos e do equipamento para conferir uma reconstrução e sustentar um relatório.”

## 4. Testes executados e limitações

### Evidências de validação

| Verificação | Resultado desta revisão |
| --- | --- |
| Lógica de investigação e salvamento | 3.446 verificações; 1.632 estados explorados; todas as 16 configurações de modo e pulsos do novo enigma exercitadas. |
| Apresentação e capítulos | 46 verificações, incluindo introdução, pausa, eventos, retomada e exigência da reconstituição antes do capítulo de conclusão. |
| Mistura de áudio PCM | 38 verificações sem depender de um dispositivo de saída; isso valida dados e comportamento, sem equivaler a uma avaliação auditiva. |
| Execução integrada | 218 verificações aprovadas em Release e Debug, repetidas no executável instalado; zero falhas. |
| Modelos e materiais | 37 modelos do cenário mais a mesa, totalizando 38 exportações; modelos aceitos sem fallback nas verificações. |
| Compatibilidade com a base de aula | As cinco cenas 0, 1, 5, 6 e 2 foram verificadas sem erro OpenGL. |
| Quadros reais do renderizador | 33 capturas, incluindo a inspeção 3D e a interface do console. |
| Desempenho | Teste curto no RTX 3050 Laptop: 1280 × 720, MSAA 4×, 2 s de aquecimento + 3 s de amostra, 880 quadros; mediana 2,5398 ms e percentil 95 de 5,8479 ms por quadro (CPU + GPU, sem VSync, alvo interno). É uma verificação curta, não um ensaio prolongado ou garantia em outro PC. |

O fluxo integrado verifica deslocamento com colisões, acesso às pistas, energia, rádio, chave, comando físico da lente, rejeição de configurações incorretas, confirmação, salvamento do rascunho, continuação e encerramento. Também cobre a preservação de progresso ao visitar menus, rejeição de arquivo corrompido, persistência de opções e reinício.

Os testes de estados e configurações verificam as regras programadas e a existência de continuação nos percursos cobertos. Não comprovam que uma pessoa encontrará todas as pistas sem dificuldade. As capturas são evidência visual do que o renderizador produziu; não representam uma sessão humana completa de jogo.

Uma prévia com perfil técnico isolado também verificou os botões reais: continuar, consultar o caderno, abrir o console pelo registro, recusar a tentativa errada, confirmar o ensaio, bloquear seus controles, avançar ao capítulo 4, girar/inclinar o diário e restaurar a vista. Não houve audição manual nem percurso humano completo. Os hashes dos arquivos de progresso e opções reais permaneceram iguais antes e depois.

### Limitações que permanecem

- **Validação com pessoas:** ainda é necessário observar jogadores sem orientação, medir compreensão dos enigmas e ajustar a ajuda. Não foi realizada uma pesquisa com participantes nesta etapa.
- **Acabamento artístico:** o cenário continua pequeno, com exterior e duas salas. Ainda há espaço para melhorar arquitetura, materiais, desgaste, transições do litoral e variedade de objetos. As inspeções mostram os modelos existentes, não novos objetos de arte detalhada.
- **Personagens e animações:** não foram produzidos personagens animados, mãos em primeira pessoa, dublagem ou cenas com atores. Os eventos apresentados utilizam animações de equipamentos, ambiente e interface.
- **Recursos gráficos:** a base usa OpenGL, reflexos por cubemap e soluções próprias de luz e sombra. Não foi convertida em um motor com ray tracing ou conjunto completo de materiais PBR. O bake confirmado continua restrito ao piso.
- **Áudio:** os sons são sintetizados pelo programa. Os testes PCM não substituem ouvir a mistura em fones e caixas de som reais, conferir conforto e ajustar níveis no computador da apresentação.
- **Narrativa e avaliação:** existe uma investigação principal e um conjunto de hipóteses para o relatório; esta revisão não acrescenta campanhas ou finais ramificados. O programa exige evidências e justificativa preenchida, mas a qualidade argumentativa do texto ainda precisa ser avaliada por uma pessoa.
- **Desempenho:** uma medição no computador de desenvolvimento não garante o mesmo resultado em outros equipamentos. É necessário experimentar a versão empacotada na máquina usada para a apresentação.

### Organização para explicar o código

| Parte do projeto | Responsabilidade principal |
| --- | --- |
| `Investigation.h` e `Narrative.h` | Regras dos enigmas, descobertas, perguntas, ajuda, documentos e condições do relatório. |
| `UltimoSinal.cpp` | Movimento, mira, interação, abertura de telas e coordenação da sessão. |
| `Interface.cpp` | Menus, leituras, caderno, mecanismos e configurações. |
| `Inspection.cpp` e `Inspection.frag` | Renderização dos modelos em vista de inspeção e reprodução local do ensaio. |
| `Storage.h` e código de persistência | Codificação, validação e recuperação de progresso e opções. |
| `Experience.h` e `Experience.cpp` | Prólogo, capítulos, eventos e apresentação derivados do progresso. |
| `World.cpp`, `Prototype.frag` e `Sky.frag` | Montagem do cenário, materiais, iluminação e apresentação ambiental. |
| `Audio.h` e `Audio.cpp` | Ambiente, passos, equipamentos, efeitos e mistura de som. |
| `RuntimeChecks.cpp` e projetos de testes | Verificação das regras, persistência, integração e renderização. |

A base C++/OpenGL e as cenas do professor foram mantidas. O trabalho usa produção nativa no 3ds Max e scripts assistidos por Codex; a madeira de aula continua entre os recursos utilizados. Os arquivos `.max` são fontes editáveis; modelos `.3DS`, texturas e shaders são recursos usados pelo jogo. A revisão deve ser apresentada como uma versão em desenvolvimento com melhorias demonstráveis e limites conhecidos.

## Entrega local desta revisão

- Executável instalado: `BaseAppOGL/bin/InvestigacaoRelease/BaseAppOGL.exe`, aberto por `Jogar.cmd`.
- Pacote atualizado: `dist/O-Ultimo-Sinal-Investigacao-v3.zip`, com o mesmo executável e o novo shader de inspeção. A versão anterior foi preservada em `dist/backups`.
- Distribuição validada a partir de outra pasta: 218 verificações sem falhas, 38 modelos e bitmaps com hashes iguais aos da origem. Recurso obrigatório ausente produz diagnóstico e saída de erro 2.
- O conjunto editável do Max já existente permanece em `dist/Cenario-3dsMax-Fontes.zip`; esta etapa reutilizou seus modelos, sem produzir novos arquivos `.max`.
- Testes de CPU reproduzíveis em `tools/Testar.py`: lógica, apresentação e áudio. Também podem ser abertos os projetos em `tests` no Visual Studio.
- Registros desta revisão: `logs/deep_refine_2026-10-21`; conferência do pacote: `logs/mesa_package_verificacao.json`.

