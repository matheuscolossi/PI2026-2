#pragma once
#include <array>
#include <cstddef>
namespace ultimo {
enum class ClueId : std::size_t { Diary, Tool, Footprints, Key, Photograph, Count };
constexpr std::size_t ClueCount=static_cast<std::size_t>(ClueId::Count);
struct ClueText {const char* name;const char* description;};
inline constexpr std::array<ClueText,ClueCount> ClueTexts{{
 {"Diário de Augusto", "Há três anos, Helena, filha de Augusto, morreu no naufrágio junto aos recifes. A última página diz: ‘Os mesmos três flashes longos. Comparei meus registros com a fotografia: era o sinal dos desembarques clandestinos. O navio de Helena seguiu aquela luz. Recebi outra ameaça, mas amanhã levarei as provas ao continente. Não autorizarei mais nenhum sinal.’"},
 {"Ferramenta quebrada", "Um alicate de corte está com a lâmina partida. Ao lado dele, Augusto havia deixado material de manutenção. Na fixação interditada do corrimão, um fragmento encaixa na parte que falta à lâmina. O cabo de segurança tem um corte recente, sem ferrugem na superfície exposta. A proteção da escada foi cortada."},
 {"Pegadas molhadas", "Marcas molhadas ligam o cais à sala de serviço. As solas têm sulcos em zigue-zague. As botas de Augusto, junto à entrada, possuem marcas circulares. Pelo menos outra pessoa passou por aqui na noite da morte."},
 {"Chave da sala da lente", "Uma pequena chave caiu junto às pegadas. Sua etiqueta diz ‘SALA DA LENTE’. Ela abre a passagem para o mecanismo de sinalização. Quem a carregava podia chegar ao equipamento do farol. A segunda sala agora está acessível."},
 {"Fotografia rasgada", "Os dois pedaços mostram o barco Aurora descarregando caixas no cais da ilha. No verso, Augusto anotou: ‘Aurora — três flashes longos: desembarque liberado’. A data é anterior ao naufrágio de Helena. Na margem: ‘A mesma sequência registrada na noite em que perdi minha filha.’ A identidade das pessoas na imagem não é legível."}
}};
inline constexpr std::array<const char*,3> Hypotheses{{
 "Augusto sofreu um acidente enquanto preparava um pedido de socorro.",
 "Augusto foi morto para impedir uma denúncia; outra pessoa usou o farol para orientar o desembarque clandestino.",
 "Augusto sabotou o próprio farol para vingar a filha e morreu durante a tentativa."
}};
inline constexpr const char* Intro="Augusto foi encontrado morto ao pé da escada de manutenção. Na noite anterior, o farol emitiu três flashes longos e apagou. Recolha os registros na Ilha das Brumas e descubra se a queda foi mesmo um acidente. A escada está interditada; investigue as duas salas e o caminho ao cais.";
inline constexpr const char* Explanation="A sabotagem aparece no cabo cortado e no fragmento da ferramenta. As pegadas diferentes das botas de Augusto e a chave mostram a presença de outra pessoa com acesso à lente. O diário registra ameaças e a intenção de impedir os sinais. A fotografia liga os três flashes ao contrabando anterior ao naufrágio. Em conjunto, as evidências sustentam o homicídio para encobrir a operação. Elas não permitem identificar com certeza o assassino.";
}
