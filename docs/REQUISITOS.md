# Checklist de requisitos

## Produto / PI

| Requisito | Estado | Critério de aceite |
|---|---|---|
| Ilha e duas salas navegáveis | Implementado em blocos; teste integral pendente | Percurso sem bloqueios indevidos. |
| Cinco pistas distintas | Regras implementadas e testadas | Todas alcançáveis e examináveis na cena. |
| Caderno com releitura | Implementado | Consultar textos descobertos; reinício limpa estado. |
| Chave para segunda sala | Regra testada; percurso pendente | Porta bloqueia antes e libera após chave. |
| Conclusão e feedback | Pendente | Alternativas e explicação baseadas em gabarito revisado. |
| Duração de 5–10 minutos | Pendente | Medir sessões reais, incluindo leituras. |
| Piloto com cinco adultos propostos | Pendente | Protocolo, participantes elegíveis e registros reais. |
| Resultados e limitações | Pendente | Relatório coerente com versão e dados coletados. |
| Fonte no forge | Pendente | Destino e prazo confirmados; entrega verificada. |

## Computação Gráfica — na cena do grupo

| Requisito | Estado | Evidência necessária |
|---|---|---|
| Modelagem no 3DS Max / .3DS | Pendente | Arquivo de produção e exportação funcional. |
| Unwrap UVW | Pendente | UVs no Max e textura correta no jogo. |
| Point e Spot | Pendente | Comparação com fontes ligadas/desligadas. |
| Multitextura | Pendente no jogo; exemplo na Scene5 preservado | Mistura/composição aplicada ao cenário próprio. |
| Skybox | Pendente | Céu contínuo durante rotação. |
| Environment mapping | Pendente | Material reage à direção de observação. |
| Reflexão e refração | Pendente | Efeitos distintos, não somente transparência. |
| Normal mapping | Pendente | Comparação do mesmo material com/sem mapa. |
| Fog | Pendente | Neblina varia com distância sem impedir leitura. |
| Blending | Pendente | Transparências corretamente ordenadas. |
| Bake de sombras estáticas | Pendente | Produção no 3DS Max e aplicação no jogo. |

Recursos adicionais como PhysX, partículas, animações, som e sombras dinâmicas não substituem os requisitos obrigatórios.

## Engenharia e entrega

- [x] Base original preservada com identificação e SHA-256.
- [x] SDK local configurado para Debug x64.
- [x] Cena do grupo separada das cenas de aula.
- [x] Testes de regras e interseção geométrica.
- [x] Compilação e verificação visual inicial.
- [ ] Release x64 e bibliotecas redistribuíveis compatíveis.
- [ ] Erros de recursos tratados e diagnosticáveis.
- [ ] Pacote testado fora do ambiente de desenvolvimento.
- [ ] Desempenho: 1280×720, p95 ≤33,3 ms como meta interna; três coletas de 120 s após 30 s de aquecimento.
- [ ] Direitos/autoria dos modelos e texturas documentados.
- [ ] Prazos específicos de PI confirmados.
- [ ] Ensaio no computador de apresentação.

CG 2024T2: 23/11/2026 conforme documento consultado. Os marcos anteriores de PI precisam de confirmação. Fonte: https://drive.google.com/file/d/1ysIhN584aX6Ye_HJ-5MpSdSHo0grpPk3/view
