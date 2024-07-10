## Tópicos
* [Descrição](#1)
	* [Funcionamento](#1-1)
	* [TinyLibrary](#1-2)
* [Linha de comando](#2)
	* [Bandeiras](#2-1)
	* [Estruturas](#2-2)
	* [Produto final](#2-3)
* [Regras](#3)
	* [Primeira](#3-1)
	* [Segunda](#3-2)
	* [Terceira](#3-3)
	* [Quarta](#3-4)
	* [Quinta](#3-5)
* [Problemas conhecidos](#4)
	* [Primeira](#4-1)
	* [Segunda](#4-2)
	* [Terceira](#4-3)

<br>
<h2 id="1">Descrição</h2>

&emsp; **L**ua L**i**brary Co**m**pactor, ou apenas **Lim**, é um programa de terminal criado com o intuito de facilitar a compressão de códigos escritos em [`Lua`](https://lua.com "Site oficial da linguagem") para uso no projeto [*TinyLibrary*](#1-2) (mas podendo ser usado em qualquer outro projeto que use `Lua`), de maneira à manter o funcionamento do código e o exportando em um formato denominado de *Pacote Local*.

<h3 id="1-1">Funcionamento</h3>
&emsp; O processo de compactação do Lim é relativamente simples e consiste na renomeação dos nomes de variáveis, tabelas e funções *desprotegidas* (e criadas pelo usuário) por nomes mais simples, **geralmente** formados por apenas um caracter.

``` lua
	-- example
	local function add(value1, value2)
		return value1 + value2
	end
```

``` lua
local function a(b, c) return b + c end
```

&emsp; Além dessa renomeação, em tal processo, nomes de funções nativas de Lua (como: `type`) são substituídos por um *código*, que nada mais é do que uma variável que referencia a função em questão, mas com uma menor quantidade de caracteres em seu nome, o que, a longo prazo, faz uma diferença considerável no tamanho do código.

``` lua
error("Message")
type(false)
print("Hello World"!)
```

``` lua
E3("Message")
T21(false)
P11("Hello World!")
```

&emsp; Após esse processo, nem tudo será compactado, pois tal ato geraria a quebra da lógica sintática e semântica do código. A exceção à compactação se aplica às:

* Globais *de ambiente*: que só podem ser acessadas e manipuladas dentro do *ambiente* da biblioteca e por funções da mesma.
* Globais *puras*: que podem ser acessadas e manipuladas por qualquer códig presente no programa (que tenha sido escrito após a *implementação* da biblioteca)
* Funções *internas*: só podem ser chamadas dentro do *ambiente* da biblioteca.
* Funções da bibliotecas: são indexadas à tabela da biblioteca, podendo serem chamadas foram de seu *ambiente*.

###### Veja também: [Regras](#3)

<h3 id="1-2">TinyLibrary</h3>
&emsp; Esse é o nome dado à uma coleção de bibliotecas para o console/computador de fantasia [Tic80 Tiny Computer](https://tic80.com "Site oficial"), que visa a facilitação e abstração de diversos recursos do mesmo e da disponibilização de códigos úteis.

&emsp; Uma grande peculiaridade do Tic80 é seu limite de caracteres (`65.536` para a versão base e `524.288` na versão PRO), o que acarretou na criação do *formato/versão compactado(a)* para as bibliotecas de tal coleção, para que assim usuários tivessem mais espaço para escrever o código do seu projeto e para mantê-lo limpo.

&emsp; Inicialmente, esse processo era manual, mas com o crescimentos dos códigos da bibliotecas, tornou-se necesária a criação de uma ferramenta auxiliar, assim obtendo uma melhor eficiência.

> [!IMPORTANT]\
> Atualmente, o suporte que o Lim possui ao Tic80 não pode ser desativado, ou seja, códigos fornecidos à ele, que não irão ser usados no Tic80, não devem conter nomes indênticos aos da API de tal programa. Felizmente, em futuras atualizações, esse suporte tornará-se opcional.

###### [Veja mais](https://github.com/duckafire/TinyLibrary "Repositório oficial")

<br>
<h2 id="2">Linha de comando</h2>
&emsp; Assim como todo programa de terminal, Lim possui algumas *bandeiras* e estruturas de organização, que devem ser usadas para executar funções do mesmo e customizar seus resultados.

<h4 id="2-1"> Bandeiras</h4>
* `-v`: Imprime a versão em execução do Lim e sua licença.
* `-h`: Imprime diversas informações sobre o Lim, como bandeiras e [regras](#3).
* `-r`: Força a substituição de arquivos, caso já exista um com o mesmo nome do [produto final](#2-1).

> [!NOTA]\
> `-r` não substitui o arquivo de origim

> [!NOTA]\
> Caso `libName` não seja especificado, `origin` será usado em seu lugar.

<h4 id="2-2">Estruturas</h4>
* `lim [-h|-v]`
* `lim [-r] <origin>.lua [<libName>]`

> [!TIP]\
> Geralmente, em estruturas de código para programas de terminal, palavras entre `[ ]` são opcionais e entre `< >`são palavras variam sua escrita. Já o `|` indica a possibilidade de uso de usadas das suas opções vizinhas.

##### Por exemplo
`lim library.lua`
`lim -r animation.lua AN`

> [!TIP]\
> Especifique nomes curtos para `libName` (`AN` no exemplo), para assim economizar mais espaço no código e depois, caso desejar, renomeie o arquivo, para que assim possa ser mais fácil identificá-lo e manipula-lo (AN.limfile -> animation.limfile)

> [!IMPORTANT]\
> Não especifique argumentos (`origin` e `libName`) arquivos nomeados com [palavras reservadas](https:///github.com/duckafire/LIM/blob/main/info/RESERVED_WORDS.txt "Lista de palavras reservadas do Lim").

<h4 id="2-3">Produto final</h4>
&emsp; Esta é a denominação dada ao produto final gerado pelo sucesso do processo de compactação, sendo esse um arquivo de texto, com a extensão `limfile`. Tal arquivo conterá todo o contéudo compactado, pronto para *copiar e colar*.

&emsp; Sua estrutura é a seguinte:

``` lua
local TL={}
do local MA0=math.abs TL.abs=function(a) return MA0(a) end end
local lib=TL
```

```
~    +--> Table that will be store the library functions
~    |
~ +---------+
1 local TL={}
~
2 do local MA0=math.abs TL.abs=function(a) return MA0(a) end end
~ ++ +----------------+ +----------------------------------+ +-+
~  |    |                                           |         |
~  |    +--> References to LUA functions            |         |
~  |                                                |         |
~  |                     Function of the library <--+         |
~  +----------------------------------------------------------+
~                 |
3 local lib=TL    +--> Contention to the library environment
~ +----------+
~      |
~      +--> Reference to the library
```

> [!TIP]\
> O uso da uma referência não é obrigatório, mas é recomendado, já que possibilita uma maior economia caso a biblioteca em questão tenha um nome grande (como [`Magic_Palette`](https://github.com/duckafire/TinyLibrary/tree/main/collection/compacted "Biblioteca da TinyLibrary")).

<br>
<h2 id="3">Regras</h2>
&emsp; Como forma de orientar o Lim durante a compactação, se faz necessário seguir uma série de regras semânticas, as quais visam (além de orientar o programa) explicitar a maneira com a qual determinados valores comportarão-se ao fim do processo e evitar a quebra do código. Ambas estão listadas abaixo:

<h4 id="3-1">1 - Funções com nome prefixado por <code>LIB_</code> (na declaração e chamadas) serão adicionadas a tabela da biblioteca.</h4>
&emsp; Dessa forma, fica explicito, tanto para o Lim quanto para o desenvolvedor, quais funções utilizam recursos da própria biblioteca.

> [!NOTE]\
> Após a compactação, `LIB_` será substituído por `libName`.

<h4 id="3-2">2 - Variáveis e tabelas com the prefixo `local` (ou [`_G`](https://www.lua.org/manual/5.3/manual.html#pdf-_G "Documentação oficial da Lua 5.3")) declaradas no início da linha, não serão compactadas.</h4>
&emsp; Isso foi pensado para tornar possível o armazenamento persistente de valores durante o período de execução do programa, de modo a quê funções da biblioteca possa manipulá-los e fornece-los aos usuários.

> [!IMPORTANTE]\
> Funções declaradas no início das linhas, que não são prefixadas por `LIB_` também não serão compactadas.

> [!NOTE]\
> Variáveis e tabelas criadas com `local` só poderão ser acessadas por funções dentro do *ambiente* da biblioteca (mas pode ser acessadas através do uso de funções), já aquelas criadas com [`_G`](https://www.lua.org/manual/5.3/manual.html#pdf-_G "Documentação oficial da Lua 5.3") poderão ser acessadas com qualquer linha de código posterior à biblioteca.

> [!TIP]\
> Declare multiplas variáveis usando uma única palavra reservada (`local`), da seguinte maneira: `local values, effect, code = {}, false, 0x03FF8`

<h4 id="3-3">3 - Não crie cadeias de caracteres com `[[ ]]`.</h4>
&emsp; A principal funcionalidade desse delimitador é a sua capacidade de converter quebras de linha que ocorrem em seu interior em quebras de linha para a cadeia em questão, logo, como após a compactação realizada pelo Lim o código passa a ocupar apenas uma linha, este perde seu uso principal e diferencial.

> [!TIP]\
> Use o [Caractere de Escape](https://en.m.wikipedia.org/wiki/Escape_sequences_in_C "Página da Wikipédia sobre Caracteres de Escape") `\n`, adjunto com `'` ou `"`, para quebrar linhas  em suas cadeias de caracteres.

<h4 id="3-4">4 - Não use <code>@</code> no código.</h4>
&emsp; Durante o processo de compactação, esse caractere é usado para deixar diversas marcações no código, criada automaticamente pelo Lim para que o mesmo possa se orientar em futuras ações. Dito isso, é possível considerar essa como sendo uma ["palavra" reservada](https:///github.com/duckafire/LIM/blob/main/info/RESERVED_WORDS.txt "Lista de palavras reservadas do Lim").

> [!IMPORTANT]\
> Seu uso *pode* acarretar em resultados e problemas imprevisíveis.

> [!NOTE]\
> Em futuras atualizações, o sistema de marcações do Lim será refinado, o que resolverá todo e qualquer problema que o uso desse caracter possa gerar.

<h4 id="3-5">5 - Não finalize nomes de funções, variáveis e tabelas com dois ou mais números seguidos.</h4>
&emsp; Nomes que não seguem essa regra serão considerados, pelo Lim, como sendo número.

<br>
<h2 id="4">Problemas conhecidos</h4>
&emsp; Está versão (`0.1.11`) do Lim é muito estável e usável, entretanto está à mercê de alguns problemas, os quais geram códigos "defeituosos", porém que felizmente podem ser corrigidos de maneira fácil e (muitas vezes) rápida (mas apenas manualmente).

<h4 id="4-1">1 - As vezes lixo é gerado próximo ao fim do código.</h4>
&emsp; Um meio de barra esse lixo foi criado, mas ele não é 100% eficiente, então, eventualmente, algum lixo pode acabar indo parar no [produto final](#2-3). Ele consiste em alguns poucos (0-15) caracteres aleatórios.

<h4 id="4-2">2 - Caminhos de arquivo não são suportados, apenas o nome do arquivo.</h4>
&emsp; Ou seja, caso `origin` seja especificado como `src/origin.lua` ele será denotado como inválido, nesse caso, se faz necessário estar *dentro* do directório do arquivo em questão.

<h4 id="4-3">3 - Valores hexadecimais são quebrados pelo processo de compactação.</h4>
&emsp; Em outras palavras, tais códigos numéricos não são compactados, mas perdem parte de seus caracteres, o que gera uma quebra considerável na lógica do código.

> [!TIP]\
> `local var = tonumber(0x00)` ou `local var = string.format("%x", "0x00")` podem ser usado para evitar o 3º problema.

> [!IMPORTANTE]\
> Versões futuras do Lim trarão correções para ambos os problemas.