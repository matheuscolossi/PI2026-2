# Medição técnica — 07/10/2026

Três execuções **Release x64**, resolução 1280×720, VSync solicitado ligado, quatro amostras de antialiasing solicitadas e todos os efeitos do jogo ligados. Cada execução teve 30 segundos de aquecimento e 120 segundos de amostragem.

GPU: NVIDIA GeForce RTX 3050 Laptop GPU. Driver informado pelo OpenGL: 4.6.0 NVIDIA 610.74. Visual Studio Community 2026, toolset v145. A mediana próxima de 6,94 ms é compatível com atualização de aproximadamente 144 Hz; não é evidência de taxa máxima sem VSync.

| Execução | Quadros amostrados | Mediana (ms) | p95 (ms) |
|---|---:|---:|---:|
| 1 | 17278 | 6.9426 | 7.1527 |
| 2 | 17292 | 6.9427 | 7.3066 |
| 3 | 17291 | 6.9449 | 7.2148 |

Os três p95 ficaram abaixo da meta interna de 33,3 ms. Isso vale para esta versão, esta máquina e a trajetória técnica medida. Não certifica a arte final, outros computadores, a experiência manual ou a duração de 5–10 minutos.

## Método e limitações

O modo `--benchmark` percorre repetidamente o eixo do cais às duas salas, variando a orientação da câmera, com porta aberta e HUD. Mede o intervalo completo entre inícios de quadros, incluindo renderização e troca de buffers. Não reproduz cliques, leitura prolongada ou resposta de um participante. O caderno e o formulário não compõem essa trajetória.

As execuções ocorreram no computador em uso. A visibilidade/foco da janela não foi mantida continuamente sob controle exclusivo; houve troca de foco e inspeções durante o trabalho. São resultados técnicos locais, sujeitos à carga e ao gerenciamento de energia do sistema. Repetir em condições controladas no computador de apresentação.

Arquivos brutos: `logs/benchmark_1_frames.csv`, `benchmark_2_frames.csv`, `benchmark_3_frames.csv`. Saída de cada execução: `logs/benchmark_1.log` a `benchmark_3.log`. Resumo: `logs/benchmark_summary.json`.

SHA-256 do executável medido: `865ed3ff8e01017d6c3b9185ae638e146f2965a834f8183be704cb64fca0bbb8`.

Para repetir: compilar Release, fechar atividades concorrentes relevantes, abrir `BaseAppOGL.exe --benchmark` três vezes, preservar o CSV após cada execução e registrar GPU/driver, foco, energia e versão. O CSV é sobrescrito a cada execução. Não usar estes registros como respostas ou tempos do piloto acadêmico.
