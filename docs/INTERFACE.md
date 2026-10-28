## Revisão atual — 22/10/2026: cenário e percurso vertical

Escada em espiral com 36 degraus transitáveis, abertura real na cobertura, patamar e sala óptica acessível. Três áreas conectadas no mesmo cenário: sala de serviço, arquivo e sala óptica. Acesso ao armário liberado, móveis reposicionados e 14 exports do Max atualizados, incluindo rádio, escrivaninha, diário, cadeira e fotografia. Conjunto: 39 modelos do cenário sem fallback mais mesa.3ds (40 arquivos); quatro mesas e uma escrivaninha.

Release/Debug e executável instalado: 306 verificações por execução, zero falhas. Lógica: 3.446/1.632 estados; apresentação: 46; áudio PCM: 38; suporte espacial: 185. Há 38 quadros de revisão e cinco cenas sem erro GL. Apenas o piso recebeu novo CompleteMap. Madeira e fotografia geradas são declaradas em CREDITOS.md. Progresso real preservado.

Procedimento, apresentação, alterações no código, evidências e pendências: [Revisão do cenário](Revisao_cenario_22-10-2026.md). Os blocos datados abaixo são históricos; seus totais e posições antigas não descrevem a versão atual.

## Revisão atual — 21/10/2026: investigação e inspeção

Três enigmas conectados: energia, rádio e novo ensaio local do comando da lente. Foram integrados console físico, inspeção 3D manipulável de quatro evidências, caderno agrupado com horários conhecidos, volumes de interação e áudio por distância/direção. Rascunhos e confirmações do console são salvos; partidas antigas incompletas preservam descobertas e relatórios já concluídos permanecem imutáveis.

Release/Debug e executável instalado: 218 verificações, zero falhas; lógica: 3.446 verificações/1.632 estados; apresentação: 46; áudio PCM: 38. Há 33 quadros do renderizador, 37 modelos do cenário aceitos sem fallback mais a mesa e cinco cenas de aula sem erro GL. Prévia dos controles feita em perfil isolado, sem alterar o progresso real. Ainda há acabamento artístico e avaliação com jogadores pendentes.

Guia, explicação do código, validação e limites: [Revisão profunda](Revisao_profunda_21-10-2026.md). Os registros datados abaixo preservam etapas anteriores e seus totais antigos.

# Interface de investigação

A interface deixa o ambiente visível durante a exploração. A mira é pequena e a identificação do objeto só aparece ao alcance. O painel de contagem e o objetivo que revelava a chave foram removidos.

## Telas e ações

- **Início:** continuar progresso existente ou sessão ativa em memória, nova investigação, opções, controles e saída. Continuar fica indisponível quando não há nenhum dos dois. Uma nova investigação exige confirmação antes de substituir o progresso, mesmo se houve falha ao gravar.
- **Exploração:** indicação discreta do caderno e da pausa; o comando contextual usa a tecla remapeada de interação. Avisos de ações recusadas aparecem temporariamente.
- **Leitura:** documento central com quebra de linha, rolagem e retorno à origem. A transmissão possui transcrição integral e não depende da audição.
- **Caderno:** pergunta pendente e somente registros descobertos. Não lista objetos ainda não encontrados e não revela automaticamente a configuração do painel ou a frequência correta. Todas as descobertas podem ser relidas.
- **Quadro elétrico:** três interruptores, resposta ao rearmar, consulta ao caderno, ajuda e saída. O menu não informa a combinação correta.
- **Rádio:** seleção de canal, escuta, alimentação e transcrição persistente após a descoberta. Há consulta ao caderno e ajuda.
- **Ajuda:** orientação, indicação específica e solução são passos solicitados separadamente. Não há escalada automática de dicas.
- **Relatório:** hipótese, evidências descobertas e justificativa. Exige os pré-requisitos da investigação e dois registros selecionados. O rascunho é salvo a cada cerca de cinco segundos mesmo durante digitação. O programa não corrige semanticamente o texto.
- **Desfecho:** explica as relações entre registros, preserva a resposta anterior ao resultado e reconhece a descoberta opcional da carta. O relatório enviado pode ser consultado, mas não editado após o feedback. As evidências não identificam o executor.
- **Pausa:** retomar, salvar, opções, controles, ajuda, início e saída. Sair solicita salvamento antes de encerrar.

## Opções e controles

Volumes geral, ambiente e efeitos são independentes. Sensibilidade varia de 0,015 a 0,20; há inversão vertical, redução de movimentos, brilho de 0,75 a 1,35 e três níveis de qualidade, com Média como padrão. Os ajustes são persistidos ao aplicar. Restaurar opções restaura também as teclas padrão.

Seis comandos podem ser remapeados usando letras de A a Z. Captura e rejeição de teclas duplicadas são realizadas no processamento de entrada da cena; a interface inicia ou cancela a captura. Tab/Esc, F1 e F2 mantêm pausa, ajuda e opções. Atalhos acadêmicos não são apresentados como comandos de jogo.

## Apresentação e acessibilidade

Painéis azul-marinho, texto em tom de papel e realces âmbar mantêm a identidade do farol. A janela central tem altura limitada à resolução e rolagem nativa. Instruções respeitam o remapeamento. Informação essencial do rádio é textual e pode ser relida no caderno. Falta de dispositivo de áudio é comunicada nas opções sem impedir a investigação.

Mensagens de salvamento são exibidas, inclusive erros. Se o disco falhar, a sessão em memória é mantida enquanto o aplicativo está aberto; o menu não a confunde com um salvamento persistido. A cena solicita gravação ao ser destruída. Cada botão chama uma regra existente, muda um estado ou persiste configurações/progresso.

## Verificação

A revisão de cinco mesas foi compilada em Debug e Release em 20/10/2026, em saídas distintas. Os testes puros permanecem em 436 verificações/184 estados; a verificação atual do executável passou em **58 verificações sem falhas**, cinco cenas GL0 e dez quadros. Logs e hashes estão em `VALIDACAO.md`. O modelo real é reutilizado em cinco instâncias; não há mudança nas regras de investigação por causa dessas instâncias.


A ferramenta Sky, após autorização, conferiu na janela: brilho 1,00 → 1,20 persistindo após reabrir; sensibilidade 0,060 → 0,079 → 0,060; qualidade Alta → Média → Alta; rolagem até Aplicar; recusa de W duplicado para Examinar; Q aceito e persistido após reabrir; restauração de E/WASD/J. Também foram conferidos cancelar Nova investigação, Continuar, J para caderno, F1 com ajuda inicial sem solução, Esc, Tab para pausar/retomar e Sair pela pausa.

O arquivo de opções terminou idêntico ao backup original. O progresso não foi alterado pelos menus/reinícios anteriores a Continuar. Backups estão em `work/ui_backup_2026-10-19`. O bloqueio inicial de acesso foi resolvido e não permanece como impedimento dessa verificação.

Esta conferência não inclui interação Q em um alvo real, caminhada/colisões por comandos mantidos, audição, percurso humano completo ou avaliação com participantes. A ausência de salto aparente na pausa é uma observação das imagens, sem certificar precisão ou conforto do mouse físico.


A prévia estática autorizada `--preview-tables` confirmou visualmente as mesas do diário e da manutenção na sala inicial. Ela usou perfil isolado e preservou os hashes de progresso/opções; Esc encerrou normalmente. O registro `logs/mesas_cinco_preview_verificacao.json` não representa sessão jogada e não amplia as conclusões anteriores sobre conforto, interação ou percurso humano.
