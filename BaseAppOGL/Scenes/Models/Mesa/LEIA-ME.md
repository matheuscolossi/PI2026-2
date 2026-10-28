## Acabamento de 21/10/2026

As cinco Boxes originais continuam editáveis, com modificadores Chamfer e Unwrap acrescentados no Max. Os modelos e imagens de checker da etapa inicial são registros históricos. A versão anterior sem acabamento está no backup antes_polimento; o fonte atual e o export possuem o acabamento de bordas. Produção: Cenario/polish_models.ms e Cenario/evidencias/polish_native.txt.

# Mesa do farol — arquivos para estudar e editar

Abra **mesa.max no 3ds Max 2027**. A cena possui cinco Boxes editáveis: Tampo e Perna1–Perna4. As unidades de exibição e de sistema são metros: 1 unidade = 1 metro. O conjunto mede 1,60 × 0,90 × 1,00 m no Max, com Z para cima.

Cada peça mantém Unwrap UVW; o mapeamento usa Flatten Mapping. Para estudar, selecione uma peça, abra Modify → Unwrap UVW → Open UV Editor. O material Standard usa Bitmap `wood.jpg` no canal Diffuse.

Esta produção foi realizada no Max com **MAXScript nativo assistido pelo Codex**. `criar_mesa.ms` é destinado somente a uma cena vazia e recusa substituir modelos existentes. Não execute novamente a produção na cena já criada. Para editar o trabalho existente, abra o .max e salve uma cópia antes de alterar.

`wood.jpg` é cópia de `P1.jpg` do material da aula; não é textura autoral nem bake. A origem e o hash estão em `ORIGEM_TEXTURA.txt`.

## Evidências

- `evidencias/modelo_editor.jpg`: cena e objetos no editor.
- `evidencias/uv_editor.jpg`: editor de UV.
- `evidencias/uv.png`: template do **tampo**, renderizado nativamente no Max em 1024 × 1024.
- `evidencias/checker.png`: conferência das cinco peças com quadrados proporcionais. A densidade difere entre tampo e pernas.
- `producao_max.txt` e `conferencia_max.txt`: produção, recuperação da tentativa inicial e reabertura nativa assistida.

A madeira foi restaurada antes de salvar/reabrir; `mesa.3ds` utiliza a madeira, sem o checker.

## No jogo

`mesa.3ds` e `wood.jpg` formam o recurso de execução. O importador combina as peças em uma malha de 2.940 triângulos, após o acabamento de 21/10/2026 e converte o eixo vertical. A mesa da fotografia é apoiada no piso em **(3; 0,095; −3)** no jogo, com dimensões X/Y/Z de 1,60 × 1,00 × 0,90 m. O shader utiliza as UVs importadas.

Depois de editar, exporte as cinco peças selecionadas como **3D Studio (.3DS)** e confira o resultado no jogo. Preserve o .max editável e a imagem junto da exportação. O cenário atual possui 37 modelos do conjunto Cenario, além desta mesa. O piso tem CompleteMap nativo; outras superfícies usam sombras analíticas. Acabamento e validação com pessoas continuam pendentes.
