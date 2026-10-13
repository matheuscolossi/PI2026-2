# O Último Sinal

Jogo investigativo C++/OpenGL para Projeto Integrador VI e Computação Gráfica. Base 2024T2 de 06/10/2026 UTC, com aulas 0, 1, 5 e 6 preservadas; jogo na cena 2.

## Primeira compilacao depois de clonar

O Git nao inclui `OpenGL_SDK`: cada integrante precisa configurar sua copia do SDK do professor. Use Visual Studio Community 2026 com desenvolvimento desktop em C++, toolset v145 e Windows SDK.

Na raiz do repositorio, execute no PowerShell, indicando onde extraiu o SDK:

```powershell
& .\tools\ConfigurarSDK.ps1 -Origem 'C:\OpenGL_SDK'
& .\tools\PrepararSDKDebug.ps1
```

O primeiro script copia as dependencias para `OpenGL_SDK`, ao lado de `BaseAppOGL`, sem alterar a pasta original nem as variaveis globais do Windows. O segundo compila as bibliotecas Debug a partir dos fontes originais (precisa das ferramentas CMake do Visual Studio). Depois abra a solucao e compile **Debug | x64**. As DLLs Debug sao copiadas automaticamente para a pasta do executavel.

Para compilar **Release | x64**, prepare antes as bibliotecas Release (precisa das ferramentas CMake do Visual Studio):

```powershell
& .\tools\PrepararSDKRelease.ps1
```

Debug usa as bibliotecas construidas em `OpenGL_SDK/debug`; Release usa `OpenGL_SDK/release`. Ambos reutilizam os fontes do SDK do professor. Nao misture as duas configuracoes. O SDK permanece fora do Git. Se o projeto ja estava aberto durante a configuracao, recarregue a solucao antes de compilar.

## Abrir e executar

1. Abra `BaseAppOGL/BaseAppOGL.sln` no Visual Studio Community 2026.
2. Selecione **Release | x64** para jogar ou **Debug | x64** para depurar.
3. Use **Ctrl+F5**. Para abrir a versão já compilada, execute `Jogar.cmd`.

O SDK está ao lado da solução em `OpenGL_SDK`. Debug e Release usam bibliotecas compiladas dos fontes locais do SDK, em `OpenGL_SDK/debug` e `OpenGL_SDK/release`. O executável encontra os recursos pela própria localização e pode ser iniciado de outra pasta.

O pacote local está em `dist/O-Ultimo-Sinal-Windows-x64`. Extraia/copie a pasta inteira, incluindo Scenes, Fonts e DLLs. Teste também no computador de apresentação. Requer Windows x64, driver com OpenGL 4.6 e Microsoft Visual C++ Runtime x64 compatível; o pacote não instala componentes automaticamente.

## Controles

| Tecla | Ação |
|---|---|
| Enter na abertura | Iniciar |
| WASD / mouse | Andar / olhar |
| E | Examinar / fechar leitura |
| J | Caderno; clique numa pista descoberta para reler |
| Tab | Pausa / voltar à tela anterior |
| R na pausa ou no resultado | Reiniciar |
| Enter após cinco pistas | Abrir formulário de conclusão |
| Esc | Sair; durante digitação de justificativa os atalhos globais são bloqueados |
| 0, 1, 5, 6 | Aulas do professor |
| 2 | Recriar a cena do jogo |
| V / Caps Lock | VSync / wireframe, herdados da base |

As opções de câmera estão na abertura e na pausa: sensibilidade, inversão vertical e restaurar padrão. As configurações duram até fechar/recriar a cena. A pausa oferece comparações dos efeitos gráficos.

## Investigação

Augusto morreu após uma queda no farol. A chave encontrada perto das pegadas abre a sala da lente. Examine diário, ferramenta, pegadas, chave e fotografia. Ao reunir tudo, escolha uma hipótese, marque duas evidências e explique sua conclusão antes de receber o feedback. O programa não corrige semanticamente a justificativa; no piloto, o observador registra e avalia a resposta.

## Organização

- `Scenes/UltimoSinal/Narrative.h`: conteúdo e hipóteses.
- `Investigation.h`: regras e estados.
- `UltimoSinal.cpp`: entrada, movimento, colisão e seleção.
- `Interface.cpp`: menus, caderno, opções e relatório.
- `World.cpp` e shaders: geometria e materiais.
- `Engine/MouseMotion.h`: acumulação de eventos, testável sem janela.
- `tests`: 5.068 verificações de regras e geometria.
- `docs`: requisitos, narrativa, origem da base, resultados e pendências.

## Testes

`tools/Testar.ps1` ou o projeto `tests/Testes.vcxproj`: testes puros. O executável aceita `--verify-game` para percorrer automaticamente a geometria real e renderizar todas as cenas. Aceita `--benchmark` para 30 s de aquecimento e 120 s de coleta em 1280×720, VSync ligado; escreve `benchmark_frames.csv` na raiz dos recursos. O modo de benchmark usa uma trajetória técnica repetível e não é piloto com jogadores.

Consulte `docs/VALIDACAO.md`, `docs/DESEMPENHO.md` e `docs/STATUS.md` para evidências e limites.

## O que ainda falta

Modelagem autoral no 3DS Max, Unwrap UVW, sombras por bake, detalhes finais das pistas, comparação visual rigorosa dos efeitos, validação manual completa e piloto com cinco participantes. Os gráficos melhoraram, mas os modelos atuais continuam provisórios. A duração de 5–10 minutos ainda não foi confirmada.

Repositório publicado em 13/10/2026: [matheuscolossi/PI2026-2](https://github.com/matheuscolossi/PI2026-2), branch `main`. O histórico local foi preservado. O SDK, os backups e os pacotes compilados permanecem fora do Git. Os créditos e limitações de licença estão em `docs/CREDITOS.md`. Nenhum resultado de participante foi simulado.
