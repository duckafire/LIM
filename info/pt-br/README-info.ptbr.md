<h2 id="0">Tópicos</h2>

* [Descrição](#1)
	* [Funcionamento](#1-1)
	* [*TinyLibrary*](#1-2)
* [Linha de comando](#2)
	* [Bandeiras](#2-1)
	* [Estruturas](#2-2)
	* [Produto final](#2-3)
* [Regras](#3)
	* [Declaração de funções](#3-1)
	* [Declaração de variáveis e tabelas](#3-2)
	* [Criação de cadeias de caracteres](#3-3)
	* [Caractere reservado](#3-4)
	* [Últimos caracteres](#3-5)
* [Problemas conhecidos](#4)
	* [*Lixo* eventual](#4-1)
	* [Caminhos não suportados](#4-2)
	* [Quebra de hexadecimais](#4-3)
		
<br>

<h2 id="1">Descrição</h2>

&emsp; **L**ua L**i**brary Co**m**pactor, ou apenas **Lim**, é um programa de terminal criado com o intuito de facilitar a compactação de códigos escritos em [`Lua`](https://lua.com "Site oficial da linguagem") para uso no projeto [*TinyLibrary*](#1-2) (mas podendo ser usado em qualquer outro projeto que use `Lua`), de maneira à manter o funcionamento do código e exportando-o em um formato denominado de *Pacote Local*.

<h3 id="1-1">Funcionamento</h3>

&emsp; O processo de compactação do Lim é relativamente simples e consiste na renomeação dos nomes de variáveis, tabelas e funções *desprotegidas* (e criadas pelo usuário) por nomes menores, *geralmente* formados por apenas um caracter.

``` lua
-- example
local function add(value1, value2)
	return value1 + value2
end
```

``` lua
local function add(a,b) return a+b end
```

&emsp; Além dessa renomeação, em tal processo, nomes de funções nativas de Lua (como: `type`) são substituídos por um *código*, que nada mais é do que uma variável que referencia a função em questão, mas com uma menor quantidade de caracteres em seu nome, o que, à longo prazo, faz uma diferença considerável no tamanho do código.

| Original | Referência |
| :--      | :--        |
|error("Message")<br>type(false)<br>print("Hello World"!) | E3("Message")<br>T21(false)<br>P11("Hello World!") |

&emsp; Após esse processo, nem tudo será compactado, pois tal ato geraria a quebra da lógica sintática e semântica do código. A exceção à compactação se aplica às:

* Globais *de ambiente*: que só podem ser acessadas e manipuladas dentro do *ambiente* da biblioteca e por funções da mesma.
* Globais *puras*: que podem ser acessadas e manipuladas por qualquer código presente no programa (que tenha sido escrito após a *implementação* da biblioteca).
* Funções *internas*: só podem ser chamadas dentro do *ambiente* da biblioteca.
* Funções da bibliotecas: são indexadas à tabela da biblioteca, podendo ser chamadas foram de seu *ambiente*.

###### Veja também: [Regras](#3)

<h4 id="1-2">TinyLibrary</h4>

&emsp; Esse é o nome dado à uma coleção de bibliotecas para o console/computador de fantasia [Tic80 Tiny Computer](https://tic80.com "Site oficial"), que visa a abstração de diversos recursos do mesmo e disponibilização de códigos úteis.

&emsp; Uma grande peculiaridade do Tic80 é seu limite de caracteres (`65.536` para a versão base e `524.288` na *versão PRO*), o que acarretou na criação do *formato/versão compactado(a)* para as bibliotecas de tal coleção, para que assim usuários tivessem mais espaço para escrever o código do seu projeto.

&emsp; Inicialmente, esse processo era manual, mas com o crescimentos dos códigos das bibliotecas, tornou-se necesária a criação de uma ferramenta auxiliar, para assim obter uma maior eficiência.

<br>

> [!IMPORTANT]\
> Atualmente, o suporte que o Lim possui ao Tic80 não pode ser desativado, ou seja, códigos fornecidos à ele, que não irão ser usados no Tic80, não devem conter nomes idênticos aos da [API](https://github.com/nesbox/TIC-80/wiki/API "API do Tic80 - Wiki") de tal programa. Felizmente, em futuras atualizações, esse suporte tornará-se opcional.

<br>

###### [Veja mais](https://github.com/duckafire/TinyLibrary "Repositório oficial da TinyLibrary")

<br>

<h2 id="2">Linha de comando</h2>

&emsp; Assim como todo programa de terminal, Lim possui algumas *bandeiras* e estruturas de organização, que devem ser usadas para executar funções e customizar resultados do mesmo.

<h4 id="2-1"> Bandeiras</h4>

* `-v`: Imprime a versão em execução do Lim e sua licença.
* `-h`: Imprime diversas informações sobre o Lim, como bandeiras e [regras](#3).
* `-r`: Força a substituição de arquivos, caso já exista um com o mesmo nome do [produto final](#2-1).

<br>

> [!NOTE]\
> `-r` não substitui o arquivo de origem. 

<br>

> [!NOTE]\
> Caso `libName` não seja especificado, `origin` será usado em seu lugar.

<br>

<h4 id="2-2">Estruturas</h4>

* `lim [-h|-v]`
* `lim [-r] <origin>.lua [<libName>]`

<br>

> [!TIP]\
> Geralmente, em estruturas de código para programas de terminal, palavras entre `[ ]` são opcionais e entre `< >`são palavras cuja escrita pode variar. Já o `|` ("*ou*") indica a possibilidade de escolha entre o uso de suas opções vizinhas.

<br>

##### Por exemplo

* `lim library.lua`
* `lim -r animation.lua AN`

<br>

> [!TIP]\
> Especifique nomes curtos para `libName` (`AN` no exemplo), para assim economizar mais espaço no código, e depois, opcionalmente, renomeie o arquivo, para que assim possa ser mais fácil identificá-lo e manipulá-lo (`AN.limfile` -> `animation.limfile`).

<br>

> [!IMPORTANT]\
> Não especifique uma [palavra reservada](https:///github.com/duckafire/LIM/blob/main/info/RESERVED_WORDS.txt "Lista de palavras reservadas do Lim") para `libName`.

<br>

<h4 id="2-3">Produto final</h4>

&emsp; Produto final, biblioteca,  arquivo compactado ([...]) são algumas das denominações dadas ao arquivo gerado pelo sucesso do processo de compactação, sendo esse um arquivo de texto com a extensão `limfile`. Tal arquivo conterá todo o contéudo compactado, pronto para *copiar e colar*.

&emsp; Sua estrutura é a seguinte:

``` lua
local TL={}
do local MA0=math.abs TL.abs=function(a) return MA0(a) end end
local lib=TL
```

```
~    +--> Tabela que armazenará as funções da biblioteca
~    |
~ +---------+
1 local _L={}
~
2 do local MA0=math.abs TL.abs=function(a) return MA0(a) end end
~ ++ +----------------+ +----------------------------------+ +-+
~  |    |                                           |         |
~  |    +--> Referência para funções Lua            |         |
~  |                                                |         |
~  |                        Função da biblioteca <--+         |
~  +----------------------------------------------------------+
~                 |
3 local lib=_L    +--> Contenção para o ambiente da biblioteca
~ +----------+
~      |
~      +--> Refência à biblioteca
```

<br>

> [!TIP]\
> O uso da uma referência não é obrigatório, mas é recomendado, já que possibilita uma maior economia de espaço, principalmente quando a biblioteca em questão possui um nome grande, como [`Magic_Palette`](https://github.com/duckafire/TinyLibrary/tree/main/collection/compacted "Biblioteca da TinyLibrary").

<br>

<h2 id="3">Regras</h2>

&emsp; Como forma de orientar o Lim durante a compactação, se faz necessário seguir uma série de regras semânticas, as quais visam (além de orientar o Lim) explicitar a maneira com à qual determinados valores serão tratados e evitar a *quebra* do código. Ambas estão listadas abaixo:

<h4 id="3-1">1 - Funções com nome prefixado por <code>LIB_</code> (na declaração e chamadas) serão adicionadas a tabela da biblioteca.</h4>

&emsp; Dessa forma, fica explícito, tanto para o Lim quanto para o desenvolvedor, quais funções utilizam recursos da própria biblioteca.

<br>

> [!NOTE]\
> Após a compactação, `LIB_` será substituído por `libName`.

<br>

<h4 id="3-2">2 - Variáveis e tabelas com o prefixo <code>local</code> (ou <a href="https://www.lua.org/manual/5.3/manual.html#pdf-_G"><code>_G</code></a>) e declaradas no início da linha, não serão compactadas.</h4>

&emsp; Isso foi pensado para tornar possível o armazenamento persistente de valores durante o período de execução do programa, de modo à quê funções da biblioteca possam manipulá-los e fornecê-los aos usuários.

<br>

> [!IMPORTANT]\
> Funções declaradas no início das linhas, que não são prefixadas por `LIB_`, também não serão compactadas.

<br>

> [!NOTE]\
> Variáveis e tabelas criadas com `local` só poderão ser acessadas por funções dentro do *ambiente* da biblioteca (mas podem ser acessadas através do uso de funções), já aquelas criadas com [`_G`](https://www.lua.org/manual/5.3/manual.html#pdf-_G "Documentação oficial da Lua 5.3") poderão ser acessadas com qualquer linha de código posterior à biblioteca.

<br>

> [!TIP]\
> Declare multíplas variáveis usando uma única palavra reservada (`local`), da seguinte maneira: `local values, effect, code = {}, false, tonumber(0x03FF8)`.

<br>

<h4 id="3-3">3 - Não crie cadeias de caracteres com <code>[[ ]]</code>.</h4>

&emsp; A principal funcionalidade desse delimitador é a sua capacidade de converter quebras de linha que ocorrem em seu interior em quebras de linha para a cadeia em questão, logo, como após a compactação realizada pelo Lim a biblioteca passa a ocupar apenas uma linha, esse delimitador perde seu diferencial.

<br>

> [!IMPORTANT]\
> Lim não *respeita* cadeias de caracteres delimitadas por eles (`[[ ]]`).

> [!TIP]\
> Use o [Caractere de Escape](https://en.m.wikipedia.org/wiki/Escape_sequences_in_C "Página da Wikipédia sobre Caracteres de Escape") `\n` para quebrar linhas  em suas cadeias de caracteres.

<br>

<h4 id="3-4">4 - Não use <code>@</code> no código.</h4>

&emsp; Durante o processo de compactação, esse caractere é usado para deixar diversas marcações no código, para que o Lim possa se orientar em futuras ações. Dito isso, é possível considerar essa como sendo uma ["palavra" reservada](https:///github.com/duckafire/LIM/blob/main/info/RESERVED_WORDS.txt "Lista de palavras reservadas do Lim").

<br>

> [!IMPORTANT]\
> Seu uso *pode* acarretar em resultados e problemas imprevisíveis.

<br>

> [!NOTE]\
> Em futuras atualizações, o sistema de marcações do Lim será refinado, o que resolverá todo e qualquer problema que o uso desse caracter possa gerar.

<br>

<h4 id="3-5">5 - Não finalize nomes de funções, variáveis e tabelas com dois ou mais números seguidos.</h4>

&emsp; Nomes que não seguem essa regra serão considerados, pelo Lim, como sendo número.

<br>

<h2 id="4">Problemas conhecidos</h4>

&emsp; Está versão (`0.1.11`) do Lim é muito estável e usável, entretanto está à mercê de alguns problemas, os quais geram códigos "defeituosos", porém que felizmente podem ser corrigidos de maneira fácil e (muitas vezes) rápida (mas apenas manualmente).

<h4 id="4-1">1 - As vezes lixo é gerado próximo ao fim do código.</h4>

&emsp; Há um meio de barrar esse lixo foi criado, mas ele não é 100% eficiente, então, eventualmente, algum lixo pode acabar indo parar no [produto final](#2-3). Esse lixo consiste em alguns poucos (0-15) caracteres aleatórios.

<h4 id="4-2">2 - Caminhos de arquivo não são suportados, apenas o nome do arquivo.</h4>

&emsp; Ou seja, caso `origin` seja especificado como `src/origin.lua` ele será denotado como inválido, nesse caso, se faz necessário estar *dentro* do directório do arquivo em questão.

<h4 id="4-3">3 - Valores hexadecimais são quebrados pelo processo de compactação.</h4>

&emsp; Em outras palavras, tais códigos numéricos não são compactados, mas perdem parte de seus caracteres após a compactação, o que gera uma quebra considerável na lógica do código.

<br>

> [!TIP]\
> `local var = tonumber(0x00)` ou `local var = string.format("%x", "0x00")` podem ser usado para contornar o 3º problema.

<br>

> [!IMPORTANT]\
> Versões futuras do Lim trarão correções para ambos os problemas acima.

<br>

### [Voltar ao topo](#0)
