# Recursos e autoria

Atualização: 22/10/2026. Os documentos datados preservam as etapas anteriores e os resultados válidos em cada data.

O código do jogo em Scenes/UltimoSinal foi desenvolvido para o projeto com assistência do Codex. A produção do conjunto de modelos e UVs utilizou MAXScript nativo gerado com a mesma assistência e executado no 3ds Max 2027. Não representa modelagem manual integral do aluno. O master atual é Cenario/cenario.max; Mesa/mesa.max mantém a mesa editável. Há 39 exports do cenário mais mesa.3ds, quatro instâncias dessa mesa e uma nova escrivaninha no rádio. Grupos anteriores ocultos no master e backups preservam o histórico.

Engine, cenas de aula, SDK, grass.bmp, rock.jpg, rockNormal.jpg, P1.jpg, arial.ttf e materiais de exemplo vieram da base/material fornecido pelo professor. O céu e Castle.obj vieram da cópia local BaseAppOGL_Skybox desse material. A procedência original e a licença de redistribuição dessas imagens/modelos/fonte não estavam explicitadas nos arquivos consultados. Não atribuir esses recursos ao grupo. A estátua .max/.3ds/.fbx é exemplo da Scene6 e não conta como produção do grupo.

Bibliotecas: GLFW, GLEW, GLM, Assimp, FreeType, Dear ImGui e stb, presentes no SDK/base. Avisos disponíveis em LICENSES e nos fontes do SDK.

## Imagens geradas nesta revisão

**wood.jpg**, nas pastas Cenario e Mesa, é a textura de madeira com veios finos gerada pela ferramenta de imagens integrada do Codex. **photo.jpg** é uma imagem ficcional em sépia do navio Aurora, criada pela mesma ferramenta. Não é uma fotografia histórica real nem uma evidência documental externa. Os prompts originais estão em Cenario/evidencias/spatial_2026-10-22/wood_prompt.txt e aurora_prompt.txt. Os JPEGs finais usados pelo jogo ficam ao lado de cenario.max. A fotografia foi aplicada à malha por UV explícito no Max.

A antiga wood.jpg era uma cópia de Scenes/Textures/P1.jpg da aula, hash 2a6b31601595f94d4d33f24095be9611b45f364d692051c4ed8eb307d3c1145b. Ela foi substituída nesta revisão; P1.jpg original e o backup anterior foram preservados. Os registros anteriores que indicam essa igualdade de hashes são históricos. ORIGEM_TEXTURA.txt e ORIGEM_IMAGENS.txt documentam os mapas atuais.

Os bitmaps iron.jpg, paint.jpg, paper.jpg, brass.jpg, shoe.jpg, chalk.jpg, rust.jpg, stone.jpg, grass.jpg, black.jpg e blue.jpg em Cenario foram criados por mapas Noise nativos do Max e renderizados para arquivos locais. Suas cores e parâmetros foram definidos no MAXScript assistido; não são fotografias externas ou pintura manual. Converter Noise em bitmap não equivale a bake de iluminação/sombras.

## Bake e efeitos

Somente o piso recebeu CompleteMap 1024 × 1024 em UV1, produzido no Max com Render To Texture/Default Scanline. O p_bake.jpg atual foi refeito para a escada, o layout, a nova madeira e as luzes desta revisão. A comparação de sombras ligadas/desligadas alterou 322.424 pixels acima de 12 níveis RGB. Hashes, escopo e evidências em Cenario/evidencias/bake_verificacao.json. Porta, porta do armário e chave foram excluídas como projetoras dinâmicas desse mapa estático.

O shader 13 lê o CompleteMap sem relighting Point/Spot; os demais objetos continuam com iluminação e sombras analíticas. Não houve bake de todos os objetos. Reflexo/refrator utiliza o céu estático, sem captura dinâmica dos objetos. Os fallbacks procedurais permanecem para desenvolvimento; a revisão validou 39/39 imports nativos com zero fallback.

O áudio é sintetizado localmente por Audio.cpp, incluindo mar, vento, passos e equipamentos. Não usa gravações externas ou dublagem. As informações essenciais do rádio possuem transcrição.

## Referência e arquivos

O vídeo fornecido pelo usuário, [Room & Exits Escape Room Wicked Games — level 2](https://www.youtube.com/watch?v=Q-eZ4QR0ObA), orientou composição, distribuição e leitura dos objetos. Nenhuma imagem ou modelo desse jogo foi copiado.

Cenario-3dsMax-Fontes.zip reúne masters, scripts, exports, mapas e evidências. O pacote jogável omite .max, .ms e pastas de evidências. Os scripts de revisão preservam os caminhos/estado da estação de produção; para editar a versão atual, abrir o master e resolver os bitmaps locais se necessário. Consulte [Revisão do cenário](Revisao_cenario_22-10-2026.md) para procedimento, validação e limites.
