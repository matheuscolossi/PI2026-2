## Sanidade de 08/11/2026

BENCHMARK framebuffer_nonuniform=1 first_and_last=checked
BENCHMARK complete=1 mode=offscreen-msaa4 warmup=2s sample=3s vsync=not_applicable resolution=1280x720 count=595 median_ms=5.1977 p95_ms=6.7245
GPU=NVIDIA GeForce RTX 3050 Laptop GPU/PCIe/SSE2 Driver=4.6.0 NVIDIA 610.74

Amostra curta offscreen 1280×720/MSAA4, 2 s de aquecimento e 3 s de coleta. Não inclui apresentação no monitor. Coleta completa e teste em outra máquina permanecem pendentes. Log: logs/art_2026-11-08/benchmark_smoke.log. Registros abaixo são históricos.

## Sanidade da revisão de 22/10/2026

BENCHMARK framebuffer_nonuniform=1 first_and_last=checked
BENCHMARK complete=1 mode=offscreen-msaa4 warmup=2s sample=3s vsync=not_applicable resolution=1280x720 count=855 median_ms=2.9148 p95_ms=5.1958
GPU=NVIDIA GeForce RTX 3050 Laptop GPU/PCIe/SSE2 Driver=4.6.0 NVIDIA 610.74

Amostra curta de renderização interna, 1280×720/MSAA4, 2 s de aquecimento e 3 s de coleta; não mede apresentação no monitor. Registro: logs/spatial_2026-10-22/benchmark_smoke.log. Medição completa e teste no computador da apresentação permanecem pendentes. Dados abaixo são históricos.

## Sanidade da revisão de 21/10/2026

Teste offscreen MSAA4 em 1280×720: 2 s de aquecimento e 3 s de coleta, 989 quadros, mediana 2,3927 ms e p95 5,0883 ms na RTX 3050 Laptop, driver 610.74. O teste confirmou imagem não uniforme e conclusão normal. É uma amostra curta; não substitui medição completa nem teste na janela/computador da apresentação. Registro: logs/pro_upgrade_2026-10-21/benchmark_smoke.log. Dados abaixo são históricos.

# Medição técnica — 19/10/2026

**A coleta integral final foi concluída em qualidade Média.** Mediana de 3,0136 ms e p95 de 6,6885 ms no método de renderização interna descrito abaixo. A coleta anterior em janela foi invalidada e está identificada separadamente.

A meta interna é **p95 de até 33,3 ms**, associada à configuração, máquina e método medidos. No protocolo atual, trata-se do tempo serializado de produção de um quadro em renderização fora da tela, sem incluir apresentação no monitor.

## Protocolo atual: renderização em alvo interno

O modo `--benchmark` utiliza Release x64 e:

- Janela oculta, independente de a área da janela receber composição/apresentação pelo sistema.
- Alvo de renderização próprio, um FBO de **1280 × 720**, com **MSAA de quatro amostras** e buffers de cor e profundidade.
- Resolução do multisampling para outro FBO de 1280 × 720 a cada quadro.
- **30 segundos de aquecimento e 120 segundos de coleta**.
- Trajetória técnica repetível entre exterior e salas, com mudança de direção da câmera.
- `glFinish()` após desenho e resolução: o relógio mede o trabalho serializado de **CPU + GPU**, incluindo a resolução do MSAA.
- Verificação de FBO completo, suporte a quatro amostras e erros OpenGL.

A GPU termina o trabalho antes do fim da amostra. Isso evita confundir envio assíncrono de comandos com conclusão do desenho, mas também elimina a sobreposição normal entre CPU e GPU. **Não é uma medida isolada de tempo da GPU nem de taxa apresentada ao jogador.**

Não há troca de buffers como parte do trabalho medido. **VSync não se aplica a essa coleta**, e o tempo de apresentação, a composição do desktop, a taxa do monitor e a latência de entrada não são medidos.

### Verificação de imagem

Antes da coleta e depois dela, o programa lê uma grade de **5 × 5 pixels distribuídos pelo alvo resolvido**. Exige variação entre as amostras e ausência de erro OpenGL. As leituras de sanidade são realizadas fora das amostras cronometradas.

O log deve indicar `framebuffer_nonuniform=1 first_and_last=checked`, `complete=1` e `mode=offscreen-msaa4`. A verificação confirma conteúdo não uniforme nos pontos inspecionados e rejeita um alvo inteiramente uniforme. Ela não comprova a qualidade artística nem todos os pixels de todos os quadros.

### Teste curto de sanidade

`--benchmark-smoke` usa **2 segundos de aquecimento e 3 segundos de coleta** para conferir inicialização do alvo, desenho, resolução e verificação de imagem. Seus resultados **não são o benchmark final de desempenho**, não substituem os 30 + 120 segundos e não devem preencher a tabela final.

## Resultado final — 19/10/2026

| Execução integral / configuração | Quadros | Mediana (ms) | p95 (ms) | Meta interna |
|---|---:|---:|---:|---|
| 1 — Média (1), 1280 × 720, MSAA4, offscreen | 35189 | 3,0136 | 6,6885 | Atendida nesta coleta |

GPU=NVIDIA GeForce RTX 3050 Laptop GPU/PCIe/SSE2 Driver=4.6.0 NVIDIA 610.74

Executável SHA-256: `2cff7e5f24100b0a0901bd001239bf7ef59a79d5fd08a5fa0e1a4c9e05f7c8f5`. O resumo `logs/deep_final_benchmark_summary.json` identifica também os hashes dos shaders. Os arquivos brutos estão em `logs/deep_final_benchmark_1.log` e `logs/deep_final_benchmark_1_frames.csv`; uma cópia das evidências fica em `docs/evidencias_2026-10-19`.

As verificações de conteúdo antes/depois passaram (`framebuffer_nonuniform=1 first_and_last=checked`). A coleta completou 30 s de aquecimento e 120 s de amostragem. Não houve controle da carga de outros programas nem da condição de energia. Trata-se de uma coleta técnica local, sem equivalência a FPS percebido, latência ou teste humano. A execução visível e o computador da apresentação ainda precisam ser avaliados.

A qualidade padrão é Média. Alta continua disponível; configurações já salvas são respeitadas. Esta medição não aprova automaticamente Alta nem a GPU integrada.

## Coleta invalidada em janela

A execução anterior na NVIDIA GeForce RTX 3050 Laptop GPU, com executável SHA-256 `e298e9702e77d0ce22f58d1d8c6922e87fbfdda584fa5fa2bbe7fb9cf7bedf49`, registrou:

- 797.229 quadros em 120 segundos de amostragem.
- Mediana de 0,0371 ms e p95 de 0,1035 ms.

**Esses números são inválidos como desempenho do jogo.** A execução do laço de janela não assegurava trabalho de renderização efetivo. Um log com `complete=1` não era suficiente para demonstrar que o cenário estava sendo produzido com a carga esperada. Não há aprovação da meta a partir dessa coleta.

Os arquivos foram preservados com o prefixo `logs/invalid_window_deep_final_benchmark*`, incluindo log, CSV e resumo. Devem permanecer identificados como invalidados, sem substituir nem contaminar a coleta no alvo interno.

## Coletas exploratórias anteriores na Intel

Antes dos ajustes finais e da correção do método, três coletas em **Alta (2)**, Intel(R) UHD Graphics, driver **4.6.0 — Build 31.0.101.5081**, registraram:

| Execução anterior | Quadros | Mediana (ms) | p95 (ms) |
|---|---:|---:|---:|
| 1 | 4.720 | 24,4097 | 44,5028 |
| 2 | 3.335 | 34,8034 | 43,8955 |
| 3 | 5.040 | 22,9557 | 41,3200 |

Essas coletas usavam o laço de janela, com VSync solicitado, e excederam a meta interna no método então usado. São evidências exploratórias preservadas em `logs/deep_benchmark_1.log` a `deep_benchmark_3.log`, respectivos CSVs e `logs/deep_benchmark_summary.json`.

**Não são diretamente comparáveis à nova coleta NVIDIA fora da tela**: mudaram GPU, código, qualidade e protocolo. Não permitem atribuir a diferença apenas a uma otimização do jogo.

## Seleção de GPU

O executável exporta a preferência Optimus para solicitar a GPU NVIDIA de alto desempenho, sem mudar configurações globais do Windows. Outros mecanismos de seleção podem ter precedência; o dispositivo deve ser confirmado pelo `GL_RENDERER` registrado. O mecanismo e sua precedência estão descritos na [nota técnica oficial NVIDIA sobre políticas Optimus](https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf).

## Histórico resumido

A versão anterior à revisão de investigação apresentou p95 de 7,15 a 7,31 ms na RTX 3050, com outro cenário e método em janela. Esses registros históricos permanecem em `logs/benchmark_1_frames.csv` a `benchmark_3_frames.csv` e `logs/benchmark_summary.json`. Não validam a versão atual.

## Como repetir e interpretar

1. Compilar Release e registrar hashes do executável e shaders.
2. Registrar qualidade, GPU/driver, energia e carga concorrente.
3. Usar `--benchmark-smoke` apenas para sanidade do método, se necessário.
4. Executar `BaseAppOGL.exe --benchmark` até completar 30 + 120 segundos.
5. Preservar saída e `benchmark_frames.csv` com nomes próprios: o CSV é sobrescrito na execução seguinte.
6. Conferir o conteúdo não uniforme, `complete=1`, modo, amostras e ausência de erros.
7. Preencher a linha final com mediana, p95 e arquivos brutos.
8. Validar separadamente a execução visível e o computador da apresentação.

A duração do percurso técnico não é tempo de investigação de um participante. Esta coleta não mede procura de pistas, leitura prolongada, justificativa, conforto do mouse ou audição. Capturas e GL0 também não certificam fluidez visível. O piloto, a meta de 5–10 minutos e a avaliação humana continuam separados.

A coleta final foi repetida após proteger salvamentos antes de iniciar/carregar uma sessão e separar as superfícies coplanares encontradas na inspeção da janela. A coleta anterior foi preservada como `logs/pre_ui_guard_deep_final_benchmark*`; o hash acima corresponde ao executável do pacote final. Não se atribui a diferença entre coletas apenas a essas mudanças, pois a carga de fundo não foi controlada.
