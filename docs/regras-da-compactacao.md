# Regras da compactação

Como forma de facilitar a leitura e interpretação do código fonte
(*tanto pelo Lim, quanto pelo usuário*), foi definica uma série de convensões semânticas,
denominadas *Regras*. Ambas estão listadas abaixo.

1. Apenas as variáveis, tabelas e funções globais serão adicionadas à biblioteca. Essa
medida foi tomada para evitar conflitos com identificadores externos à biblioteca.

2. Multíplas variáveis não devem receber multíplos valores,
durante sua declaração (*por exemplo `local func, tab, zero = ipairs(foo)`*), apenas
após ela. Veja o porquê disso em:
[Compactação explicada: içamento de identificadores](https://github.com/duckafire/lim/blob/main/docs/içamento-de-identificadores.md).

3. Palavras chave, números (*de todos os tipos*), caracteres especiais, identificadores
iniciados com `'_'` e identificadores globais **não** serão compactados.

4. Variáveis e tabelas globais devem **sempre** ser prefixadas por `_G.`. Do contrário,
elas não serão adicionadas à *tabela da biblioteca*, podendo, eventualmente, ser
confundidas com variáveis e tabelas locais.

5. O delimitador de cadeias de caracteres `[[ ]]` não é suportado. Isso ocorrer por conta
que o modelo de *organização final* adotado pelo Lim conflita com a principal
funcionalidade deste delimitador, que é sua capacidade de aceitar caracteres especiais
literais, o que inclui quebras de linha.