# Requisitos e evidências — 07/10/2026

Origem oficial de CG: [rubrica 2024T2](https://drive.google.com/file/d/1ysIhN584aX6Ye_HJ-5MpSdSHo0grpPk3/view). Data consultada para CG: 23/11/2026. Prazos específicos de PI continuam pendentes de confirmação.

## Computação Gráfica na cena do grupo

| Requisito | Origem | Implementação | Estado / evidência |
|---|---|---|---|
| Modelagem no 3DS Max / .3DS | Oficial | Importador Assimp da base preservado; objetos próprios ainda procedurais | **Pendente**: arquivos autorais .max/.3ds |
| Unwrap UVW | Oficial | Materiais atuais usam coordenadas por posição | **Pendente**: UVs produzidas no Max; projeção no shader não comprova UVW |
| Point e Spot | Rubrica Point/Spot; grupo assume ambas | Duas luzes locais nas salas e foco junto à câmera | Implementado e renderizado; comparação separada ainda pendente |
| Multitextura | Oficial | Grass + rock com mistura pelo caminho e bordas da ilha | Implementado em `Prototype.frag`; cenário conferido |
| Skybox | Oficial | Cubemap de seis faces e classe CSkybox do professor | Implementado e visível na execução |
| Environment mapping | Oficial | Amostragem direcional do cubemap em materiais | Implementado; não representa reflexão dos objetos locais |
| Reflexão e refração | Oficial | `reflect` na água; `refract` e Fresnel na lente | Implementado; validação visual isolada pendente |
| Bump ou normal | Oficial; grupo assume normal | Mapa normal de rocha e base tangente calculada por derivadas | Implementado; comparação ligado/desligado disponível |
| Fog | Oficial | Atenuação por distância e cor do horizonte | Implementado e renderizado; comparação disponível |
| Blending | Oficial | Água e lente depois dos opacos, mistura alfa, escrita de profundidade desativada | Implementado; revisar sobreposição e transparência em diferentes posições |
| Bake de sombras estáticas | Oficial | Nenhuma textura de bake autoral aplicada | **Pendente**: produzir no Max; iluminação dinâmica não substitui |

As cenas de aula demonstram conteúdo do professor. O `.max` e o `.3ds` da estátua da cena 6 não são produção do grupo.

## Projeto Integrador e escopo do grupo

| Requisito | Origem | Implementação / evidência | Estado |
|---|---|---|---|
| Ilha e duas salas | Planejamento do grupo | Volumes do cenário; percurso com colisão testado no executável | Testado automaticamente; sessão manual completa pendente |
| Cinco pistas e chave | Planejamento | IDs estáveis; 120 ordens em testes; porta antes/depois da chave | Testado |
| Caderno e releitura | Planejamento | Retorno ao caderno ao fechar uma releitura | Testado em lógica |
| Escolha e justificativa antes de feedback | Metodologia do grupo | Três alternativas, duas evidências e texto obrigatório | Testado em lógica e no percurso automatizado |
| 5–10 minutos | Meta do grupo | Cronômetro inclui leitura e pausa até envio | Pendente de piloto |
| Cinco adultos / acerto / clareza | Protocolo do grupo | Formulário do observador; sem participantes nesta etapa | Pendente |
| Resultado e limitações | PI | Documentação técnica atualizada | Relatório do piloto pendente |
| Fonte no forge | Entrega acadêmica | Git local, sem remoto configurado | Pendente |

## Engenharia

- Base original, arquivo de atualização, hash e cópia anterior preservados.
- Debug e Release x64 compilam com o SDK local; Release tem dependências próprias.
- Caminhos resolvidos a partir da localização do executável; falta de recursos essenciais produz diagnóstico.
- Testes de lógica e de geometria não certificam a experiência completa nem nota acadêmica.
- Benchmark de três execuções é medição técnica; não prova duração de jogo ou compreensão narrativa.
- Som, partículas e sombras dinâmicas são opcionais e não substituem as obrigações da rubrica.
