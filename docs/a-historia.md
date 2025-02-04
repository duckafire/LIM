# Compactação explicada
###### Escrito em: 02-03/02/2025

[//]: # "Lista de links"
[lua]:     <https://lua.org>    "Website oficial"
[love2d]:  <https://love2d.org> "Website oficial"
[tic80]:   <https://tic80.com>  "Website oficial"
[tinylib]: <https://github.com/duckafire/tinylibrary> "Repositório oficial (GitHub)"
[lang-c]:  <https://https://en.m.wikipedia.org/wiki/C_(programming_language)> "Página da linguagem na Wikipédia"
[leg-cha]: <https://duckafire.itch.io/legendary-champion> "Página do jogo na itch.io"
[git]:     <https://git-scm.com> "Website oficial"

---

* [Introdução](#introdução)
* [Nasce a ideia](#nasce-a-ideia)
* [Limites fantasiosos](#limites-fantasiosos)
* [A solução](#a-solução)
* [A base da obra](#a-base-da-obra)
* [A face do problema](#a-face-do-problema)
* [O despertar](#o-despertar)
* [Por que o nome "Lim"?](#por-que-o-nome-lim)
* [Considerações finais](#considerações-finais)

---

## Introdução

Nesta página contarei um pouco sobre a história desse pequeno projeto,
aprofundando-me principalmente no que desencadeou seu surgimento. Certamente esse não é
um conhecimento crítico para ser ter, principalmente se seu objetivo for apenas usar
ou entender como o código funciona, então sinta-se a vontade para pular essa parte.

## Nasce a ideia

[*Tic80 Tiny Computer*][tic80] é um console/computador de fantasia, que, sem sobre
de dúvidas, foi extremamente importante, tanto para meu desenvolvimento como programador,
quanto para o surgimento do Lim, pois foram suas *"limitações"* que me levaram a criar
este programa.

Com o passar do tempo, minhas habilidades como programador evoluíram cada vez mais,
a ponto de me permitir notar uma certa peculiaridade: alguns trechos de código poderiam ser
facilmente reutilizados em outros projetos, o que me proporcionaria uma maior facilidade,
agilidade e qualidade no código em questão.

> Esse pensamento originou-se na época em que eu utilizava o *framework* [*LÖVE*][love2d]
> e "perseguiu-me" após meu retorno ao [*Tic80*][tic80].

## Limites fantasiosos

Escrever uma biblioteca em [*Lua*][lua] não é, de forma alguma, uma tarefa difícil,
a menos que hajam "limites técnicos", o que era o caso. Assim como muitos outros programas
do gênero, o [*Tic80*][tic80] possui algumas limitações propositais, das quais duas com
certeza dificultariam meus planos.

* Sem suporte para multíplos arquivos.
* Limite de caracteres (`65.536` *v.FREE*; `524.288` *v.PRO*).

Logo, minhas bibliotecas para uso Nesta aplicação deveriam, principalmente, ser:
"enxutas", de modo a que não proporcionassem uma grande poluição nos códigos;
"comprimidas", assim não ocupando muito espaço no *script*, permitindo que o usuário
tivesse o máximo de espaço possível para seu projeto.

## A solução

Quanto a esse pequeno infortúnio, não havia muito a ser pensar, a solução era clara,
bastava que minhas bibliotecas possuem as características antes citadas, entretanto: *como
atribuir estas características a uma código e mantê-lo legível?*. Esse não é uma pergunta
difícil, basta ter dois códigos: um código fonte e um código *compactado*. A verdadeira
dificuldade está em como alcançar esse objetivo, *como compactar o código?*

A resposta à essa pergunta não demorou muito para me vir a mente, mas sua velocidade
de chegada denunciava o quão grotesca ela era. Simplesmente compactar o código manualmente. Era um trabalho discutivelmente exigente, que, mesmo com a ajudar da substituição de texto
(*fornecida por muitos editores de arquivos*), continuava a ser desafiador.

Minha solução, discutivelmente, não foi a melhor, mas alguns fatores me levaram a
ela, sendo eles: meu conhecimento nulo com relação a manipulação de arquivos, seja com
[*Lua*][lua], seja com qualquer outra linguagem; meu desejo por aperfeiçoar minha atenção
na escrita dos meus código, tendo em vista que errinhos bobos voltados a esse tópico me
consumiam um tempo considerável.

## A base da obra

Como todo bom programador, primeiramente, visei decidir quais seriam as regras
levadas em conta para a realização desse processo, e, também, como todo *bom programador*
eu não anotei nenhuma delas, confiando-as apenas a minha mente (*o que não é uma boa ideia,
mas felizmente, por si só, não causou problema algum*). Baseado no que recordo, listarei
estas "regras" abaixo:

* Palavras chave, números, cadeias de caracteres, *funções da biblioteca* e caracteres
especiais não serão *compactados*. Todo o resto será reduzido ao mínimo possível, que
geralmente era um caractere (*quando possível, o primeiro do identificador em questão*).

* Não haveria suporte para cadeias de caracteres delimitadas por `[[ ]]`.

* Todo o conteúdo "privado" da biblioteca deveria ser *local* e estar declarado dentro de
um bloco `do end`, para que assim não houvesse a possibilidade de ocorrer algum tipo de
conflito entre o código da biblioteca e o código do projeto que à está usado.

* Para cada função do *Padrão* [*Lua*][lua], ou da *API* do [*Tic80*][tic80], haveria uma 
"variável de referência", cujo identificador seria constituído por um sublinhado e o mínimo 
de letras maiúsculas possível. A declaração destas deveria ser feita no "topo" do código, 
após à abertura do bloco `do end` (`do local _T,_P=type,print ... _P("Hello world!")`).

* Todo o bloco `do end` deveria ser reduzido a apenas uma linha de "altura", como forma de
garantir que não houvesse nenhum tipo de "poluição visual" no projeto.

* Antes da declaração do bloco `do end`, uma tabela (*denominada "tabela núcleo"*) seria
declarada, cujo destino seria armazenar o conteúdo "público" da biblioteca. Assim como os
demais identificadores, o desta tabela também deveria ser mínimo.

* Todas as *funções da biblioteca* deveriam fazer parte da *tabela núcleo*, pois essa seria
a melhor maneira de envelopar o "ambiente" da biblioteca e manter seu conteúdo acessível.

* Abaixo do bloco `do end`, deveria ser declarada mais uma tabela, a qual receberia a
*tabela núcleo*. Como essa tabela **nunca** seria utilizada dentro do "ambiente" da
biblioteca, o tamanho do seu identificador seria desregrado, tornando-a um ótimo meio
para referenciar a biblioteca.

* Nenhuma variável, tabela ou função global deveria ser declarada dentro da biblioteca.

É certo que essa explicação possa parecer um pouco confusa, então, como forma de
superar esse probleminha, abaixo encontra-se uma representação do que, respectivamente,
seria um código fonte e um código *compactado*:

``` lua
local function message(text)
	print(text)
end
```

``` lua
local L={}
do local _P=print L.message=function(t)_P(t)end end
local lib=L
```

## A face do problema

Embarcar nessa "loucura" me trouxe muito aprendizado, melhorou minha organização,
escrita e leitura de código, entretanto também me expôs à problemas bem chatinhos e,
até mesmo, estressantes (*principalmente porque eu, nem sempre, fui tão cabeça fria quanto
sou hoje*). Estes eram causados por um "único" erro: identificadores escritos erroneamente. Por mais que a solução fosse simples, muitas vezes, era difícil identificar que este era o
problema (*e não um erro lógico ou no código fonte*), principalmente porque minha atenção
se dispersava a ponto de quase anular-se, já que um simples trecho com menos de trinta
caracteres (*por exemplo:* `x=x+s*_MC(r)y=y+s*_MS(r)`) poderia possuir tanta informação
quanto uma linha em *"código normal"*.

Porém, mesmo com todos esse probleminhas, eu ainda achava que esta era uma boa
abordagem, principalmente porque minhas bibliotecas não apresentavam grande complexidade
ou tamanho (*isso é discutível*) e por conta do exercício de atenção que essa prática
me proporcionava.

A essa altura do campeonato, eu já havia criado a [*Tiny Library*](tinylib), um
pequeno projeto que englobava todos as minhas bibliotecas para o [*Tic80*][tic80], e foi
graças a uma delas que vi-me forçado a mudar meu método.

## O despertar

Grande parte (*ou até mesmo todo*) do conteúdo presente nas bibliotecas do projeto
[*Tiny Library*][tinylib] partiram das minhas necessidades como programador, durante o
desenvolvimento do meu primeiro "jogo completo", chamado de
[*Legendary Champion: Rebirth*][leg-cha]. Na época eu precisava de um meio agradável de
manipular o acesso a memória fornecido pelo [*Tic80*][tic80], principalmente porque eu
o achava bastante confuso. Por conta disso, descidi criar uma biblioteca de controle
e "expansão" de memória, chamada *longBit*, a qual, quando comparada as demais, possuía
um nível elevado de complexidade.

Essa característica dessa biblioteca, aliada ao meu método primitivo de
*compactação*, resultou em uma bomba de *bugs*. De todo tipo, de todo tamanho, eram
problemas que não acabavam mais, erros sem fim, tanto porque meu código fonte não era
perfeito, quanto porque eu percava muito durante a compressão do código. Foi então que,
após cerca de **duas horas** (*a média girava em torno de 30-90min*) compactando um código
que resultou em erros e mais erros, eu decidi, estava na hora de aprimorar meus meios,
encontrar uma forma mais eficiente de realizar aquele trabalho desgastante, pois havia
tornando-se claro para mim o quão grande estavam e o quão grandes poderiam tornar-se
tais bibliotecas.

Nesse período, eu estava, paralelamente, iniciando meus estudos em [*C*][lang-c], e
como essa me parecia uma ótima oportunidade para obter experiências práticas com essa
linguagem formidável, eu decidi! Eu criaria um programa (*de terminal*) capaz de reduzir
um código [*Lua*][lua] ao seu "tamanho mínimo".

Foi então que no dia `04/05/2024`, no repositório da [*Tiny Library*][tinylib], eu
introduzi ao [*Git*][git] o que viria a ser a primeira versão a **primeira versão do Lim**,
denominada *Tin*. E no dia seguinte, iniciei **este** repositório, com o objetivo de criar
um *compactador de bibliotecas* [*Lua*][lua].

```
# Commit que contem a primeira versão do Lim,
# no repositório da Tiny Library

commit 8c307aa14c2f1dda04ac99503425445bfdcd05c7
Author: DuckAfire <155199080+DuckAfire@users.noreply.github.com>
Date:   Sat May 4 21:21:03 2024 -0300

    tin?
```

## Por que o nome "Lim"?

Inicialmente, eu pretendia nomear este programa como *Tin*, por conta de três motivos principais:

* Referenciava parte do "nome completo" do [*Tic80*][tic80], especificamente *Tic* e
*Tiny*.

* Este programa visava reduzir o código, deixá-lo *minúsculo*, que em inglês escreve-se
como *Tiny*.

* Era um nome curto e fácil de se lembrar.

*Mas por que eu abandonei essa ideia?* A resposta, para fazer a verdade, é um pouco
boba. Eu queria que os arquivos gerados pela aplicação tivessem seu nome como extensão, mas
ao fazer uma breve pesquisa, pude constatar que a extensão `.tin` já existia.

Por conta desse *"problema"*, decidi "buscar" um novo nome, que deveria ser composto,
também, por três letras, além de ser simples. Eu não conseguia pensar em três palavras
cujas iniciais gerassem um bom resultado, então eu mudei para uma abordagem mais criativa:
a primeira letra da primeira palavra, segunda da segunda e terceira da terceira. E daí
surgiu o nome *Lim*, oriundo de *Lua Library Compactor*.

Se você foi curioso o suficiente a ponto de procurar, deve ter notado que já havia
uma extensão de arquivo `.lim`, entretanto, o nome era tão bom e me faltava tanta
criatividade, que eu decidi manter esse. Hoje percebo que isso não era um "problema de
verdade", já que ambas as extensões são muito ninchadas e, certamente, nenhum indivíduo
confundiria os arquivos (*a menos que fosse induzido ao erro*).

## Considerações finais

Chegar até aqui foi uma tarefa difícil, cheia de problemas e desmotivações, que
felizmente me trouxe muito aprendizado, permitindo-me evoluir diversos aspectos meus,
tanto com relação a programação, quanto com relação a minha vida pessoal.

O histórico deste repositório denúncia minhas dificuldades, incertezas e erros, mas
também revela minhas soluções e maneiras, muitas vezes engenhosas (*ou apenas grotescas*),
de superar esses problemas. Essa foi, indiscutivelmente, uma aventura proveitosa, poderia
ter sido um pouco menos desgastante, mas isso não foi algo necessariamente ruim, foi
aprendizado.

Isso é tudo que você *"precisa"* saber sobre a história desse projeto. Espero que
tenha sido agradável ler esse trecho, pois escrevê-lo certamente foi. Obrigado por ler!
