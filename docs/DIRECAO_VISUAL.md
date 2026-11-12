## Revisão atual — 08/11/2026: acabamento artístico e emocional

Primeira passagem de luz, cor, desgaste, chuva e acústica nas três áreas. Reflexões após descobertas aguardam o fechamento da leitura e não alteram enigmas. Continuar não repete descobertas conhecidas. O áudio reduz apenas o ambiente durante esses momentos; pulsos e efeitos de interação são preservados. Redução de movimentos remove a chuva em queda.

Release/Debug e executável instalado: 306 verificações por execução, sem falhas. Testes: 3.446 de lógica/1.632 estados, 46 de apresentação, 44 de áudio PCM, 185 espaciais e 24 de atmosfera. Conferência: 45 quadros, 39 imports do cenário sem fallback mais a mesa. Os modelos/masters do Max e o bake do piso não foram refeitos nesta etapa. A validação emocional com pessoas e a audição real permanecem pendentes.

Detalhes, procedimentos e imagens: [Acabamento artístico e emocional](Acabamento_artistico_08-11-2026.md). Os registros datados abaixo são históricos.

## Revisão atual — 22/10/2026: cenário e percurso vertical

Escada em espiral com 36 degraus transitáveis, abertura real na cobertura, patamar e sala óptica acessível. Três áreas conectadas no mesmo cenário: sala de serviço, arquivo e sala óptica. Acesso ao armário liberado, móveis reposicionados e 14 exports do Max atualizados, incluindo rádio, escrivaninha, diário, cadeira e fotografia. Conjunto: 39 modelos do cenário sem fallback mais mesa.3ds (40 arquivos); quatro mesas e uma escrivaninha.

Release/Debug e executável instalado: 306 verificações por execução, zero falhas. Lógica: 3.446/1.632 estados; apresentação: 46; áudio PCM: 38; suporte espacial: 185. Há 38 quadros de revisão e cinco cenas sem erro GL. Apenas o piso recebeu novo CompleteMap. Madeira e fotografia geradas são declaradas em CREDITOS.md. Progresso real preservado.

Procedimento, apresentação, alterações no código, evidências e pendências: [Revisão do cenário](Revisao_cenario_22-10-2026.md). Os blocos datados abaixo são históricos; seus totais e posições antigas não descrevem a versão atual.

## Revisão atual — 21/10/2026: investigação e inspeção

Três enigmas conectados: energia, rádio e novo ensaio local do comando da lente. Foram integrados console físico, inspeção 3D manipulável de quatro evidências, caderno agrupado com horários conhecidos, volumes de interação e áudio por distância/direção. Rascunhos e confirmações do console são salvos; partidas antigas incompletas preservam descobertas e relatórios já concluídos permanecem imutáveis.

Release/Debug e executável instalado: 218 verificações, zero falhas; lógica: 3.446 verificações/1.632 estados; apresentação: 46; áudio PCM: 38. Há 33 quadros do renderizador, 37 modelos do cenário aceitos sem fallback mais a mesa e cinco cenas de aula sem erro GL. Prévia dos controles feita em perfil isolado, sem alterar o progresso real. Ainda há acabamento artístico e avaliação com jogadores pendentes.

Guia, explicação do código, validação e limites: [Revisão profunda](Revisao_profunda_21-10-2026.md). Os registros datados abaixo preservam etapas anteriores e seus totais antigos.

# Direção visual — O Último Sinal

## Identidade e composição

Uma estação costeira de trabalho, envelhecida pelo sal, com equipamentos analógicos e a aparência de um lugar usado entre as décadas de 1970 e 1980. Não é uma reconstrução histórica documental. A paleta combina cal branco-creme, pintura verde marítimo, metal escuro, telhado oxidado, madeira gasta e papel amarelado. A luz externa é fria e difusa; as luminárias internas são quentes, sem esconder os objetos importantes no escuro.

A composição do ponto inicial enquadra o caminho de lajes, o anexo com cobertura inclinada, a torre, a galeria e a lanterna. O litoral tem contorno irregular, rochas e transição entre grama e pedra. O cais tem tábuas separadas, vigas e postes de amarração. A área segura de caminhada ainda é retangular e fica contida na superfície de terra; o contorno visual não é uma simulação de terreno navegável.

## Ambientes e narrativa ambiental

Primeira sala: estação do rádio, bancada de manutenção, quadro elétrico, carta náutica, botas guardadas, armário de chaves e o fragmento interditado da escada. O armário fica no canto oeste junto à parede da entrada; a chave está numa prateleira, fora do corredor e acima do piso. A folha está fechada antes da alimentação auxiliar e aberta depois. O indicador da trava muda de vermelho para verde, e a chave desaparece da prateleira após a coleta. A carta náutica está na face INTERNA da parede frontal, acessível antes da porta intermediária.

Segunda sala: fotografia rasgada, registro, relógio parado e arquivo de equipamentos. Papel, ferramenta, botas, cabo interrompido e rádio são representações reconhecíveis; os textos e detalhes probatórios essenciais continuam acessíveis na leitura e no caderno. Os desenhos geométricos decorativos não substituem documentos legíveis.

As pegadas são três marcas incompletas sobre a pedra molhada. Não constituem uma seta ou trilha que leva diretamente à chave. A porta intermediária possui uma folha com dobradiça e abertura interpolada. O painel, o indicador do rádio e a lente respondem ao progresso real dos mecanismos.

## Materiais e luz

- Grama e pedra usam as imagens já distribuídas com a base e mistura espacial no shader. A escala é em coordenadas de mundo; o litoral e o caminho têm máscaras distintas.
- Rochas usam textura e normal map com intensidade moderada, evitando aplicar o mesmo relevo forte a toda parede.
- Cal, ferrugem, papel e madeira dos móveis procedurais usam variação de baixa intensidade. A mesa importada da fotografia usa Bitmap e UVs do Max; os materiais projetados do restante não comprovam Unwrap UVW.
- A iluminação combina ambiente frio, contribuição direcional, duas fontes Point quentes e um Spot discreto junto à câmera. A reparação elétrica altera energia e indicadores; a iluminação de emergência preserva leitura antes da solução.
- Sombras são interseções analíticas de raios de luz com caixas explícitas de paredes, teto, tampos e porta. As janelas admitem luz. O modelo aproxima os volumes selecionados: não acompanha a malha de cada parafuso, perna ou objeto. Não é shadow mapping e NÃO é o bake de sombras exigido no 3ds Max.
- O oceano é um plano, evitando as faces sobrepostas do antigo cubo transparente. As ondulações alteram apenas normais; não deslocam jogador ou câmera.
- Reflexão/refração consultam o cubemap estático da base. Não refletem a geometria local nem simulam profundidade real da água. O vidro das janelas é um plano por abertura; a lanterna usa uma casca com descarte das faces traseiras.
- O céu e a neblina compartilham uma cor de horizonte. A densidade de neblina foi reduzida para preservar silhueta e orientação.

## Opções com efeito real

Baixa qualidade reduz rochas e pequenos detalhes e remove normal mapping e sombras analíticas. Média mantém esses recursos com sombras de uma amostra. Alta acrescenta uma segunda amostra nas sombras Point para atenuar algumas bordas. Nenhuma opção troca a lógica, a disposição das pistas ou a colisão.

Brilho modifica exposição do cenário e céu. Redução de movimento fixa o tempo das normais de água; não existem balanço de cabeça ou oscilações forçadas de câmera. O tempo do mundo é fornecido pelo controlador e deve parar nos menus.

O custo das sombras foi reduzido sem remover obstáculos: faces sem contribuição solar não consultam a visibilidade dessa luz. O controlador também calcula a união das caixas de sombra realmente enviadas à GPU, com margem externa de um milímetro; raios que não alcançam essa união antes da luz dispensam o teste individual. A união é refeita por quadro e acompanha a inclusão/remoção da porta. A validação e a medição de desempenho após essa alteração são registradas no relatório geral; esta nota não atribui um ganho ainda não medido. O intervalo de brilho do cenário é de 0,75 a 1,35.

## Organização e colisão

`World.cpp::BuildWorld()` declara obstáculos grandes e `DrawWorld()` constrói sua aparência. Paredes, mesas, armário, caixas, escada e arquivo têm volumes correspondentes. Miudezas decorativas não bloqueiam o jogador. A colisão permanece em caixas simples; galeria e torre acima do teto são elementos de composição, sem acesso vertical nesta versão.

## Pendências acadêmicas e de acabamento

O cenário combina a primeira mesa produzida no 3ds Max com os demais objetos de primitivas geradas em C++. Os outros modelos autorais, seus Unwrap UVW e exportações .3DS ainda precisam substituir os elementos provisórios. As sombras analíticas não satisfazem a exigência de bake do Max. Também faltam texturas autorais para documentos e sinais, revisão artística dos objetos em close e validação visual no computador de apresentação.

### Modelo autoral de mesa aplicado às cinco bancadas

O controlador procura `Scenes/Models/Mesa/mesa.3ds` e valida seu relatório de importação: 60 triângulos, dimensões de 1,60 × 1,00 × 0,90 m em X/Y/Z, atributos finitos, UVs presentes e bitmap difuso carregado. O Assimp pode combinar as cinco peças em uma malha; o número de malhas não demonstra quantas caixas foram modeladas. O arquivo `.max`, o `.3ds` original e as evidências de produção devem demonstrar a autoria e a organização no Max.

O importador 3DS já converte o eixo vertical e o código não acrescenta outra rotação de 90 graus. Um único `CAsset` é compartilhado por cinco instâncias: diário (−3; 3), manutenção (−3; 1), registro (−3; −3), fotografia (3; −3) e rádio (3; 2,5), com coordenadas X/Z. Todas apoiam os pés em Y=0,095. Quatro preservam o tamanho do modelo; a bancada do rádio usa escala apenas em Z para atingir profundidade 1,95 m e acomodar rádio e carta. Essa variante é uma transformação da instância no jogo: o arquivo `.max`/`.3ds` autoral continua medindo 1,60 × 1,00 × 0,90 m.

Desenho, colisão e sombra percorrem a mesma lista de instâncias e seus limites medidos. O material 12 amostra o bitmap pelas UVs importadas, sem grão procedural sobre as mesas. Diário, ferramenta, documentos, rádio, xícara e fotografia têm altura visual ajustada aos novos tampos, preservando os pontos e identificadores de interação. O fallback procedural passa a ser usado nas cinco mesas somente quando o modelo é ausente ou rejeitado; não resta uma mesa provisória desenhada por cima de uma importada.

Se o arquivo não existir ou não passar pela validação, as cinco mesas usam fallback procedural e o diagnóstico informa o motivo. O teste gráfico da versão com modelo aceito exporta dez quadros: 08 destaca a fotografia, 09 a primeira sala e 10 a bancada do rádio.

Na primeira integração, limitada à fotografia, a versão Release passou 55 verificações sem falhas. O importador informou uma malha de 60 triângulos, dimensões 1,60 × 1,00 × 0,90 m e um bitmap. O quadro `08_mesa_importada.bmp` e os logs `mesa_release.log`/`mesa_runtime.log` preservam essa rodada histórica. A textura `wood.jpg` é uma cópia da aula. A ampliação para cinco mesas é validada na rodada seguinte.

Em 20/10/2026, a ampliação para cinco instâncias foi compilada em Debug e Release e passou **58 verificações sem falhas**, incluindo quantidade, posições nas duas salas, profundidade do rádio e concordância de matrizes/colisões. As cinco cenas passaram com GL0. `09_mesas_sala_inicial.bmp` e `10_bancada_radio.bmp` completam dez quadros. A prévia estática autorizada pela janela confirmou as duas mesas da primeira sala e preservou hashes do perfil, conforme `logs/mesas_cinco_preview_verificacao.json`. Isso não constitui percurso humano nem novo benchmark.

Em 20/10/2026, `verificar_mesa.ms` confirmou a reabertura nativa assistida no Max em inglês, cinco Boxes editáveis, Unwrap e Standard. O checker nas cinco peças mostrou quadrados proporcionais, com densidade distinta entre tampo e pernas. A madeira foi restaurada antes de salvar/reabrir. O template nativo do tampo, de 1024 × 1024, foi exportado em caminho ASCII e copiado byte a byte para `Scenes/Models/Mesa/evidencias/uv.png`; capturas do UV Editor e do modelo e o resumo `logs/mesa_producao_verificada.json` preservam a conferência. O pacote com a mesa também foi revalidado, conforme `VALIDACAO.md`.

Esta nota descreve a implementação, não certifica teste humano ou desempenho. Resultados de compilação, percurso e medições devem constar no relatório geral com sua data e configuração.

## Superfícies sobrepostas corrigidas

A conferência em ângulo revelou faces coplanares entre paredes, fundação e forro, além do topo da fundação coincidente com o piso de madeira. A fundação agora termina em y=0,08, abaixo do piso y=0,095. Fundação e forro medem 10,40 × 11,40, com ressaltos de 5 cm em relação às faces externas das paredes. A geometria real foi separada sem alterar colisões ou acrescentar efeitos; a exportação 07_front_oblique registra a vista de conferência.
