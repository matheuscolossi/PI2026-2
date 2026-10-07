# Verificações — 07/10/2026

## Compilação e regras

- Debug x64 e Release x64 compilados no Visual Studio Community 2026 / v145.
- 5.068 verificações aprovadas nos testes puros, incluindo 120 ordens das pistas, chave, contagem, menus, releitura, conclusão, justificativa e reinício.
- Mouse: vários eventos por quadro, limpeza, reset, movimento após recaptura e equivalência entre subdivisões em quadros testados. Sensibilidade tem limites e padrão seguro.
- Aviso C4244 preexistente em Scene5; sem erros nos builds finais.

Logs: `logs/testes_2026-10-07.log`, `tests_resultado_2026-10-07.txt`, `build_Debug_2026-10-07.log` e `build_Release_2026-10-07.log`.

## Executável e percurso

O modo `--verify-game` executou 27 verificações com as posições, colisões, movimento e seleção reais da cena: porta trancada, parede, bloqueio da fotografia através da porta, velocidade diagonal, percurso às cinco pistas, chave, conclusão, feedback e reinício.

O teste usa passos de movimento e aponta a câmera programaticamente para os alvos; não é uma pessoa jogando com mouse e teclado. Confirma acessibilidade geométrica para essa rota. A sessão manual completa e a avaliação de conforto continuam pendentes.

As cenas 0, 1, 5, 6 e 2 foram carregadas e renderizadas em Debug e Release. Todas finalizaram com erro OpenGL igual a zero. O teste inicial detectou Castle.obj ausente; o arquivo foi recuperado e os logs finais não apresentam essa falha.

Logs: `logs/runtime_checks_Debug.log` e `logs/runtime_checks_Release.log`.

## Janela

Conferidos o cenário exterior/interior, a pausa e o painel de sensibilidade/inversão. Na versão empacotada, o controle alterou a sensibilidade de 0,060 para 0,127 e o botão restaurou 0,060. Abrir/fechar o caderno preservou a direção da câmera. Alternar para o Visual Studio provocou pausa automática; voltar ao jogo preservou a direção. O movimento sintético de arrasto não demonstrou rotação sob entrada bruta, portanto não foi usado como prova de conforto/precisão do mouse físico. A nova fonte aumentou a legibilidade. Texturas, céu e geometria foram observados em execução. A aparência e o conforto do mouse ainda precisam de revisão humana; não se declara validação visual completa de todas as técnicas apenas porque o shader compila.

## Distribuição

O pacote em `dist/O-Ultimo-Sinal-Windows-x64` reúne executável Release, bibliotecas, recursos, instruções e avisos de licença. A verificação do pacote usa uma pasta de trabalho externa e o mesmo teste geométrico/cenas. Resultado e teste de ausência de recurso: `logs/package_runtime.log` e `logs/package_missing_resource.log`.

O teste negativo retira temporariamente `Scenes/Skybox/right.bmp`, exige código de saída 2 e diagnóstico com o nome do recurso e restaura o arquivo em seguida. Não remove arquivos do projeto de desenvolvimento.

Isso verifica a independência da pasta de trabalho nesta máquina. Não comprova instalação limpa sem runtime Visual C++ nem execução em outro computador. O pacote exige driver OpenGL 4.6 e runtime x64 compatível.

## Não realizados

Piloto com cinco participantes; tempos humanos; avaliação de clareza e justificativas; arte autoral .max/.3ds; Unwrap UVW; bake no Max; envio ao forge; ensaio no computador de apresentação. Esses itens permanecem no checklist.
