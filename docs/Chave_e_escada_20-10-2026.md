# Chave e escada — correção de 20/10/2026

A chave fica na prateleira do armário da primeira sala, à esquerda da entrada.
Leia a ficha de manutenção e examine as marcas perto da caixa azul no exterior.
No painel: CAIS desligado; RÁDIO e SERVIÇO ligados; Rearmar.
Com energia restabelecida, o armário abre. Mire a chave e pressione E.
O rádio não precisa estar resolvido para obter a chave.

A escada danificada foi girada 180 graus e afastada das paredes.
Os quatro topos agora estão a 0,295 / 0,495 / 0,695 / 0,895 m do zero do jogo;
o piso está a 0,095 m. São três intervalos de 20 cm, e primeiro degrau de 20 cm.
O arquivo editável do Max mantém os dez componentes originais e suas UVs.
O colisor foi ajustado para conter degraus e corrimãos, sem bloquear as pistas.
A escada continua interditada e não é acesso ao andar superior nesta versão.
Foi executado novo CompleteMap do piso com a cena corrigida; o resultado
permaneceu igual ao anterior sob essas luzes/obstáculos (diferença de pixels = 0).
A comparação de sombras ligadas/desligadas ainda diferencia 13.922 pixels.

Release e Debug: 180 verificações internas, zero falhas; 37 modelos aceitos,
zero fallbacks. Captura real do jogo: logs/deep_frames/19_escada_corrigida.bmp.
O teste inclui o percurso até a chave. Não equivale a teste com participantes.

Reabra Jogar.cmd para carregar o executável e os modelos novos.
