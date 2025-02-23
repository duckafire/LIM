# Regras da compactação

Como forma de facilitar a leitura e interpretação do código fonte
(*tanto pelo Lim, quanto pelo usuário*), foi definida uma série de convenções semânticas,
denominadas *Regras*. Ambas estão listadas abaixo.

1. Apenas as variáveis, tabelas e funções globais serão adicionadas à biblioteca. Essa
medida foi tomada para evitar conflitos com identificadores externos à biblioteca.

2. Palavras chave, números (*de todos os tipos*), caracteres especiais, identificadores
iniciados com `'_'` e identificadores globais **não** serão compactados.

3. Variáveis e tabelas globais devem **sempre** ser prefixadas por `_G.`. Do contrário,
elas não serão adicionadas à *tabela da biblioteca*, podendo, eventualmente, ser
confundidas com variáveis e tabelas locais.

4. Os delimitadores de cadeias de caracteres `[[ ]]` e `[=[ ]=]` não são suportados. Isso
ocorrer por conta que o modelo de *organização final* adotado pelo Lim conflita com a
principal funcionalidade destes delimitadores, que é sua capacidade de aceitar caracteres
especiais literais, o que inclui quebras de linha.

5. Apenas caracteres pertencentes à Tabela
[ASCII](https://www.ascii-code.com/pt "Tabela ASCII") são suportados. Os demais gerarão
*comportamentos indefinidos*.

6. Sem suporte para blocos `do end`. Isso ocorre por conta da presença de `do` em múltiplas
declarações de blocos, o que dificulta sua distinção.
