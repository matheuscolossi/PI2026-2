# Acompanhamento — 07/10/2026

## Versão atual

Implementação local na Área de Trabalho, autorizada por Matheus. Debug e Release x64 compilados no Visual Studio Community 2026. Não é a entrega acadêmica final.

| Frente | Estado | Evidência e limite |
|---|---|---|
| Base da turma | Integrada e testada em renderização | Base de 06/10 UTC: aulas 0, 1, 5 e 6; jogo separado na cena 2. `BASE_2026-10-06.md`. |
| Narrativa | Implementada | Morte de Augusto, naufrágio de Helena e sinal clandestino. `NARRATIVA.md` e `Narrative.h`. |
| Mouse | Corrigido e testado em lógica | Soma de todos os eventos, limpeza por quadro, reset na captura/foco; movimento sem multiplicar por deltaTime. |
| Ajustes de câmera | Implementados; painel conferido na janela | Sensibilidade 0,015–0,20, padrão 0,06; inversão vertical e restauração. Falta avaliação de conforto com Matheus e outros jogadores. |
| Estados e conclusão | Implementados e testados | Hipótese + duas evidências + justificativa antes do feedback; releitura retorna ao caderno; pausa restaura tela anterior. |
| Percurso geométrico | Testado automaticamente no executável | 27 verificações: deslocamento real com colisões, mira nas cinco posições, chave, conclusão e reinício. Não substitui uma sessão humana completa. |
| Cenário | Melhorado, arte provisória | Texturas, céu, farol detalhado, móveis com pernas, cais, botas e representações distintas de pistas. |
| Técnicas gráficas | Implementadas parcialmente e renderizadas | Ver matriz em `REQUISITOS.md`; comparação visual sistemática de cada efeito ainda pendente. |
| Release | Compilado e testado nesta máquina | Bibliotecas Release compiladas do SDK local; sem dependência dos DLLs Debug. |
| Distribuição | Testado nesta máquina | Pacote separado passou no percurso/cenas a partir de pasta externa; recurso ausente diagnosticado e restaurado. `VALIDACAO.md`. |
| Desempenho | Medido nesta máquina | Três execuções de 30 s + 120 s; p95 de 7,15–7,31 ms. Limites em `DESEMPENHO.md`. |
| Piloto acadêmico | Pendente | Nenhum participante ou resultado de compreensão inventado. |
| Max / UVW / bake | Pendente | Nenhum modelo autoral .max/.3ds ou bake do grupo produzido nesta etapa; 3DS Max não localizado nas instalações consultadas. |
| Repositório remoto | Publicado em 13/10/2026 | https://github.com/matheuscolossi/PI2026-2 — branch main; histórico preservado. Confirmar separadamente a submissão exigida pela disciplina. |

## Testes concluídos

- 5.068 verificações de regras, geometria, estados, resposta e movimento do mouse; 120 permutações de coleta.
- Debug e Release x64 compilados. Aviso C4244 preexistente em Scene5 preservado.
- 27 verificações do percurso geométrico no executável em ambas as configurações.
- Cenas 0, 1, 5, 6 e 2 carregadas e renderizadas com erro OpenGL igual a zero; após recuperar Castle.obj, sem erro de carregamento registrado nos logs finais.
- Inspeção da janela: cenário renderizado, pausa e opções de câmera visíveis e operantes.

## Próximos critérios de aceite

1. Jogar o percurso completo manualmente e avaliar a sensibilidade em diferentes taxas de quadros e DPIs.
2. Produzir modelos do grupo no 3DS Max, Unwrap UVW e sombras estáticas por bake; integrar pelo importador existente.
3. Conferir isoladamente Point, Spot, multitextura, normal mapping, fog, reflexão, refração e blending; guardar evidências comparativas.
4. Executar o pacote no computador de apresentação e revisar origem/licença dos recursos antes de redistribuição externa.
5. Realizar o piloto com cinco adultos, registrar respostas antes do feedback e analisar resultados reais.

O diagnóstico original está preservado em `Planejamento_Matheus.md`; estados antigos estão em `historico_2026-09-29`.
