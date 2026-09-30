# O Último Sinal

Produção reiniciada em 29/09/2026 sobre uma cópia limpa da BaseAppOGL 2024T2, atualizada pelo professor em 22/09/2026 (UTC). Esta pasta da Área de Trabalho é o novo projeto. O protótipo antigo permanece separado.

## Abrir no Visual Studio Community

1. Abra `BaseAppOGL/BaseAppOGL.sln` no Visual Studio Community 2026.
2. Selecione **Debug** e **x64**.
3. Pressione **Ctrl+F5** (executar) ou **F5** (depurar).
4. Na tela inicial do jogo, pressione **Enter**.

O nome da solução e da janela ainda segue a base do professor: BaseAppOGL / OpenGL BaseApp. O jogo é a cena 2.

O SDK está em `OpenGL_SDK`, ao lado de `BaseAppOGL`. O caminho e a pasta de execução são definidos em `BaseAppOGL/Directory.Build.props`, sem alterar variáveis globais do Windows. Não separar essas pastas.

**Release ainda não está preparado.** Use Debug x64 nesta etapa. Não executar o .exe isoladamente por duplo clique em sua subpasta: os recursos ainda dependem da pasta do projeto. `Jogar.cmd` inicia a versão já compilada com a pasta correta.

## Controles

| Controle | Ação |
|---|---|
| Enter na tela inicial | Começar |
| WASD / mouse | Andar / olhar |
| E | Examinar pista indicada pela mira / fechar leitura |
| J | Abrir ou fechar caderno; clique numa descoberta para reler |
| Tab | Pausar / continuar |
| R em pausa ou encerramento | Reiniciar |
| Enter depois das cinco pistas | Encerrar o teste do percurso |
| Esc | Sair, conforme a base |
| 0, 1 e 5 | Cenas do professor |
| 2 | Reiniciar a cena do grupo |
| V / Caps Lock | VSync / wireframe da base |

As pistas são objetos coloridos provisórios. Aproxime-se e mire neles. A chave fica na primeira sala e libera a passagem central para a segunda.

## Organização

- `BaseAppOGL/Scenes/UltimoSinal`: cena, estado da investigação e geometria de interação do grupo.
- `BaseAppOGL/Scenes/Scene0`, `Scene1`, `Scene5`: aulas preservadas.
- `BaseOriginal`: arquivo original, origem e hash SHA-256.
- `docs/Planejamento_Matheus.md`: planejamento e diagnóstico de referência.
- `docs/STATUS.md`: estado atual de cada frente e evidências de verificação.
- `docs/REQUISITOS.md`: checklist completo de PI e CG.
- `tests`: testes das regras, estados e interseção geométrica, sem janela OpenGL.
- `logs`: saídas locais de compilação, não versionadas.

## Verificações em 29/09/2026

- Base limpa e cena do grupo compiladas com MSBuild do Visual Studio Community 2026, Debug x64.
- Aviso C4244 preexistente em `Scenes/Scene5/Scene5.cpp`, sem erro de compilação.
- Testes: 120 permutações das pistas; contagem única; passagem por chave; bloqueio de coleta em menus; reinício; releitura; interseção com parede fina e raio de colisão.
- Conferência visual: tela inicial, início, caderno e cenas 0/1/5/2.
- GPU observada: NVIDIA GeForce RTX 3050 Laptop GPU, OpenGL 4.6.
- **Ainda não validado:** percurso manual das cinco pistas nesta nova cena, duração de 5–10 minutos, desempenho segundo o protocolo acadêmico e piloto.

Para repetir os testes: abra `tests/Testes.vcxproj` no Visual Studio e execute Debug x64; o resultado aparece no console. Alternativamente, execute `tools/Testar.ps1` num terminal de desenvolvimento. Esses testes não substituem a validação visual e o percurso.

## Limite desta versão

Fundação inicial: menu, cenário em blocos, cinco pistas, caderno, chave, pausa e reinício. Os shaders são cópias dos shaders de primitivas da aula; a cena do jogo ainda não demonstra as técnicas gráficas finais. A mensagem de encerramento comprova somente o estado das cinco pistas, não é a solução narrativa do mistério.

Gabarito, alternativas e feedback narrativo dependem da definição do grupo. Modelagem no 3DS Max, UVW, iluminação, multitextura no jogo, skybox, environment mapping/reflexão/refração, normal mapping, fog, blending e bake permanecem no checklist.

## Atualizações das aulas

1. Guardar cada novo arquivo da turma em `BaseOriginal` com data e hash.
2. Extrair em uma pasta de comparação, sem sobrescrever o projeto.
3. Comparar com a referência Git da base anterior (`professor-2026-09-22` nesta versão).
4. Incorporar mudanças do professor e preservar `Scenes/UltimoSinal`.
5. Reaplicar o registro da cena 2 no gerenciador se necessário.
6. Compilar e conferir aulas e jogo; registrar resultado em STATUS.

A base fica concentrada nas cenas de aula e engine. Neste início, a integração do grupo altera somente o registro da cena em `CSceneManager.cpp`, a lista de arquivos do projeto e a configuração local do SDK.

## Backup e colaboração

Há histórico Git local, sem remoto configurado e sem publicação feita. A pasta está no OneDrive; sincronização em nuvem não foi verificada. Não assumir que ela substitui uma cópia de segurança ou a entrega no forge.
