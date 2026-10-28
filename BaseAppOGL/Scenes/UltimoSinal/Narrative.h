#pragma once
#include <array>
#include <cstddef>
namespace ultimo {
enum class ClueId:std::size_t{Diary,Tool,Footprints,Key,Photograph,Count};
constexpr std::size_t ClueCount=5,EntryCount=10;
struct ClueText{const char* name;const char* description;};
inline constexpr std::array<ClueText,EntryCount> Entries{{
 {"Diário de Augusto","18 de outubro. Helena seguiu a luz errada há três anos. Descobri quem usava o Aurora. Amanhã entregarei a fotografia. Hoje disseram que a escada precisava de reparos. Não autorizei ninguém a tocar no cabo.\n\nÀ margem: 'Se a luz voltar, confiram o registro manual.'"},
 {"Alicate partido","Falta um fragmento da lâmina. Ele coincide com a peça presa no cabo de segurança recolhido na base da escada. A superfície do corte ainda está limpa. A equipe isolou o cabo danificado e fixou um corrimão provisório para o acesso à torre. Alguém preparou a queda; não foi simples ferrugem."},
 {"Marcas no acesso ao cais","Duas coisas chamam atenção: as marcas em zigue-zague não correspondem às botas de Augusto, de sola circular. E a caixa AZUL do cais está encharcada, com sal nos contatos. O caminho molhado termina na sala de serviço."},
 {"Chave de serviço","A alimentação auxiliar liberou o armário de segurança. Dentro, a chave da sala da lente: Augusto a guardava para impedir transmissões fora do turno. Ela agora está com você. A fechadura da passagem foi liberada."},
 {"Fotografia do Aurora","Uma embarcação descarrega caixas no cais. A foto é anterior ao naufrágio de Helena. No verso: 'AURORA — três pulsos longos: desembarque liberado'. Uma anotação recente liga a mesma sequência à noite do naufrágio. As pessoas na fotografia não podem ser identificadas."},
 {"Ficha de manutenção","ALIMENTAÇÃO DE EMERGÊNCIA\n\n1. Isole o ramal que teve contato com água salgada.\n2. Mantenha alimentados o RÁDIO e o SERVIÇO. O serviço libera o armário de segurança.\n3. Só então rearme a proteção.\n\nRamais: AZUL = CAIS; BRANCO = RÁDIO; ÂMBAR = SERVIÇO. A etiqueta não informa qual deles foi molhado: inspecione o acesso externo."},
 {"Carta de navegação","O canal 16 é apenas chamada de socorro. A estação de Brumas guarda registros locais no canal 6. Não use os canais de pesca para confirmar horários.\n\nNo cais, o Aurora costuma fazer desembarques sem registrar carga. Uma rota contorna os mesmos recifes do naufrágio de Helena."},
 {"Transmissão de Brumas","[Transcrição — canal 6]\n'Às 23h10 recebemos a chamada do vigia: Augusto havia caído, sem resposta. Às 23h17 registramos três pulsos longos do farol. A tempestade danificara o sistema automático às 22h40. Solicitamos conferência do registro mecânico.'\n\nA voz não identifica quem voltou a operar o equipamento."},
 {"Registro mecânico da lente","22h40 — automático desconectado por avaria.\n23h17 — comando MANUAL, três pulsos longos.\n\nA sequência não podia partir do temporizador avariado. Compare com o horário comunicado pelo rádio e com a anotação no verso da fotografia.\n\nO comando ao fundo da bancada tem um ensaio isolado: escolha a origem do comando e a duração das três posições. A lâmpada e o aviso sonoro são locais; o teste não retransmite para o mar."},
 {"Carta não enviada a Helena","'Ainda deixo a cadeira voltada para o mar. Sei que você não vai chegar no próximo barco. Se eu conseguir provar o que houve, talvez pare de procurar seu rosto em cada desembarque.'\n\nUma xícara fria e uma segunda cadeira permanecem à mesa. Esta carta é opcional: não abre nenhum mecanismo."}
}};
inline constexpr std::array<ClueText,5> ClueTexts{{Entries[0],Entries[1],Entries[2],Entries[3],Entries[4]}};
inline constexpr std::array<const char*,3> Hypotheses{{"Uma queda acidental e um sinal automático da tempestade.","Augusto foi silenciado; outra pessoa usou a luz para liberar o desembarque clandestino.","Augusto preparou a própria queda e usou o farol para se despedir."}};
inline constexpr const char* Intro="Ilha das Brumas, 05h40. Augusto foi encontrado morto junto à escada. Sete minutos depois da chamada de emergência, o farol ainda transmitiu. Descubra quem podia operar a luz — e para quem era o último sinal.";
inline constexpr const char* Explanation="O alicate e o cabo indicam sabotagem; o diário registra ameaças. Brumas recebeu a notícia da queda às 23h10. O registro confirma comando manual às 23h17, com o automático avariado desde 22h40. Portanto, outra pessoa operou a luz. A fotografia explica os três pulsos: liberar o desembarque do Aurora. As evidências sustentam o encobrimento, mas não identificam o executor.";
}
