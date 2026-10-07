# Acompanhamento — Planejamento Matheus

## Reinício autorizado em 29/09/2026

Matheus solicitou começar do zero na pasta O Último Sinal da Área de Trabalho, seguindo o planejamento e a base. Essa instrução autoriza este início de implementação e substitui o estado de espera descrito no diagnóstico histórico.

## Estado por tarefa

| Tarefa | Estado | Evidência ou pendência |
|---|---|---|
| F0.1 — Requisitos e prazos | Parcial | Checklist criado; faltam datas específicas de PI, horas disponíveis e responsabilidades. |
| F0.2 — Comparar base | Concluída | Comparação com 14/09 identificou novo Scene5 e alterações no projeto/gerenciador; arquivo atual preservado com hash. |
| F0.3 — Base atual funcional | Validada nesta máquina | Cópia limpa de 22/09, build Debug x64 e cenas 0/1/5/2 conferidas. |
| F1.1 — Narrativa/gabarito | Pendente | Conteúdo provisório; nenhuma solução do mistério definida como final. |
| F1.2 — Mapa | Em desenvolvimento | Ilha, duas salas, mesas e porta em blocos; precisa revisão do percurso. |
| F1.3 — Estados | Implementado e testado em lógica | Menu, exploração, leitura, caderno, pausa e encerramento do protótipo, sem booleanos de menus sobrepostos. |
| F2.1 — Regras e IDs | Implementado e testado | IDs nomeados, chave independente da ordem, 120 permutações testadas. |
| F2.2 — Movimento/mira | Implementado; validação parcial | Colisão, segmento/AABB e pausa ao perder foco; percurso completo ainda pendente. |
| F2.3 — Leitura/caderno | Implementado; validação parcial | Releitura disponível, caderno inicial conferido; textos finais pendentes. |
| F2.4 — Conclusão/feedback | Pendente | Apenas encerramento técnico após cinco pistas. Não confundir com conclusão narrativa. |
| F2.5 — Percurso integral | Pendente | Testes de lógica não substituem jogar até a última pista. |
| F3.1–F3.8 — Arte e gráficos | Pendente | Matriz em REQUISITOS; nenhuma técnica final certificada na cena do grupo. |
| F4.1 — Erros de recursos | Pendente | Diagnósticos herdados da base; tratamento robusto ainda falta. |
| F4.2 — Release/pacote | Pendente | Debug configurado; iniciador local facilita a execução, mas não é pacote de distribuição. |
| F4.3–F4.4 — Desempenho e versão candidata | Pendente | Sem benchmark ou validação de distribuição. |
| F5.1–F5.3 — Piloto | Pendente | Sem coleta com participantes. |
| F6.1 — Forge | Pendente | Git local; nenhum remoto ou envio. |
| F6.2 — Documentação final | Em desenvolvimento | Planejamento, README, checklist e status presentes; resultados finais faltam. |
| F6.3–F6.4 — Entrega/polimento | Pendente | Não é versão final. |

## Testes executados

Build da base original e build da cena nova passaram. O aviso C4244 da Scene5 já aparece na base limpa e foi preservado.

O executável de testes informou 4096 verificações aprovadas, incluindo as 120 ordens de coleta. Cobertura: contagem sem duplicação, chave, estados de menu, releitura somente após descoberta, bloqueio de conclusão antecipada, reinício, segmento contra parede fina, segmento paralelo e colisão com raio.

Na janela: tela inicial, Enter para iniciar, J para caderno e troca entre cenas 5, 1, 0 e 2. O caderno manteve a visão do cenário nessa verificação. Isso não prova todos os casos de foco e movimentação.

## Próximas tarefas

1. Definir narrativa e matriz pista–conclusão (F1.1).
2. Realizar o percurso manual, ajustar posições e testar chave/releitura/reinício em execução (F2.2/F2.5).
3. Confirmar acesso ao 3DS Max e validar um objeto exportado em .3DS (F3.1).
4. Integrar a multitextura ensinada na Scene5 ao terreno do jogo, preservando evidência separada de autoria e aplicação (F3.4).
5. Completar conclusão narrativa e feedback após fixar o gabarito (F2.4).

Modelos e conteúdo não devem ser considerados prontos apenas por aparecerem no planejamento.
