## Revisão atual — 08/11/2026: acabamento artístico e emocional

Primeira passagem de luz, cor, desgaste, chuva e acústica nas três áreas. Reflexões após descobertas aguardam o fechamento da leitura e não alteram enigmas. Continuar não repete descobertas conhecidas. O áudio reduz apenas o ambiente durante esses momentos; pulsos e efeitos de interação são preservados. Redução de movimentos remove a chuva em queda.

Release/Debug e executável instalado: 306 verificações por execução, sem falhas. Testes: 3.446 de lógica/1.632 estados, 46 de apresentação, 44 de áudio PCM, 185 espaciais e 24 de atmosfera. Conferência: 45 quadros, 39 imports do cenário sem fallback mais a mesa. Os modelos/masters do Max e o bake do piso não foram refeitos nesta etapa. A validação emocional com pessoas e a audição real permanecem pendentes.

Detalhes, procedimentos e imagens: [Acabamento artístico e emocional](Acabamento_artistico_08-11-2026.md). Os registros datados abaixo são históricos.

# Áudio local — O Último Sinal

O módulo `BaseAppOGL/Scenes/UltimoSinal/Audio.h/.cpp` sintetiza PCM estéreo, 16 bits, 22.050 Hz, pelo Windows WaveOut. Não baixa arquivos, não depende de serviço externo e não contém gravações de terceiros.

## Integração

- Adicionar `Audio.cpp` à compilação e `Audio.h` aos arquivos do projeto.
- Vincular **winmm.lib**. O próprio `.cpp` fornece `#pragma comment(lib, "winmm.lib")` para MSVC; configurações alternativas precisam informar essa biblioteca ao vinculador.
- Manter uma instância de `ultimo::Audio` durante a cena.
- Chamar `Update(dt, indoors, walking, master, ambient, effects)` em todos os quadros. Os volumes vão de 0 a 1. `walking` deve refletir deslocamento real, e ficar falso em pausa, menus, leitura e ao empurrar uma parede sem sair do lugar.
- `indoors` reduz o mar/vento com transição suave; não calcula propagação física ou reverberação.
- `Cue(0)` confirma inspeção; `Cue(1)` representa energia; `Cue(2)` reproduz estática e três pulsos; `Cue(3)` é porta; `Cue(4)` indica ação inválida. `pan` opcional varia de -1 (esquerda) a +1 (direita).
- Informações essenciais também precisam aparecer em texto na interface. Os sons não são requisito para compreender ou terminar a investigação.

## Sons e operação

O ambiente mistura ruído filtrado e modulações lentas para representar mar e vento. Passos alternam discretamente os canais. Interações, porta e sinal de rádio usam ruído, osciladores e envelopes curtos, sem voz e sem sustos altos.

Toda a geração e todo acesso ao dispositivo ficam em uma thread própria. A atualização por quadro apenas grava parâmetros atômicos; os eventos usam uma fila curta, protegida por mutex, com capacidade fixa. A mistura permite até 16 vozes simultâneas e descarta eventos excedentes. Não há espera por dispositivo em `Update`/`Cue`, nem novas alocações na mistura.

O módulo mantém três buffers de 512 amostras por canal (aproximadamente 70 ms de áudio na fila). Os volumes são suavizados para reduzir cliques. Master zero gera silêncio e impede novos efeitos, inclusive pulsos de rádio; pode restar somente o áudio já enviado ao dispositivo durante essa janela de buffers. Reduzir efeitos a zero descarta as vozes ativas. Não há sons guardados para tocar quando o usuário retira o mudo.

Ao destruir a cena, o módulo sinaliza encerramento, aguarda a thread, devolve os buffers e fecha dispositivo/eventos. A classe não pode ser copiada ou movida, prevenindo propriedade duplicada do dispositivo.

Sem saída de áudio compatível, `Available()` permanece falso e o jogo continua. A abertura ocorre em segundo plano; portanto, essa consulta pode retornar falso também durante os primeiros instantes de inicialização. Não há reconexão automática se o dispositivo desaparecer durante a execução.

## Limitações e validação

- É síntese procedural provisória, sem amostras gravadas de mar, atores ou equipamentos reais. Não demonstra uso de FMOD/OpenAL.
- O pan é estéreo simples; não há HRTF, áudio 3D físico nem oclusão de geometria.
- O relógio é o das amostras de áudio, independente dos quadros; `dt` permanece na assinatura para integração e não determina a velocidade dos sons.
- Escuta em fones/alto-falantes e ajuste de volume final ainda dependem de validação humana. Verificações de compilação e ciclo de vida não comprovam a qualidade sonora.
- Para verificar manualmente: caminhar dentro/fora, parar, abrir pausa, disparar cada evento, colocar master/efeitos em zero, voltar ao menu, trocar de cena e fechar o jogo. Testar também com saída de áudio desabilitada.

Verificação isolada nesta implementação: compilação MSVC C++20 com `/W4 /WX`, sem avisos, vinculando WinMM; execução de cinco ciclos de criar, atualizar, disparar todos os eventos, exceder a fila, silenciar, informar volume inválido e destruir, sem falhas. A saída de áudio abriu durante o teste. Os seis sintetizadores de efeitos (incluindo passos) também foram calculados até o término: todos produziram valores finitos e sinal não vazio, com picos individuais entre 0,078 e 0,172 antes dos controles de volume, e conversão PCM dentro do intervalo esperado. Isso confirma compilação, limites básicos do sinal e ciclo de vida nesse ambiente; não substitui escuta humana nem o teste da integração final da cena.
