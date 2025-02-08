# Como o Lim funciona?

---

* [Introdução](#introdução)
* [A arquitetura do projeto](#a-arquitetura-do-projeto)
	* [Testes unários](#testes-unários)
	* [Arquivos fonte](#arquivos-fonte)
		* [Camadas](#camadas)
		* [Mestres](#mestres)
* [Por trás dos panos](#por-trás-dos-panos)


---

# Introdução

Todo bom programa precisa de uma documentação, principalmente quando seu propósito é tão
peculiar quando o deste. Nesse artigo, pretendo explicar **todos** os processos do Lim:
como ocorrem, quando ocorrem, porque ocorrem e afins.

Esse conteúdo é destinado a todo e qualquer indivíduo que, por algum motivo, decida
estudar, modificar ou aprimorar o código deste projeto, podendo também ser aproveitado
por usuários entusiastas que queiram mais informações sobre como as coisas ocorrem
por baixo dos panos.

---

# A arquitetura do projeto

Assim como qualquer outro diretório de projeto, esse não é formado apenas por arquivos
fonte, tendo em si uma série de outros arquivos auxiliares, que servem tanto para
facilitar a compilação do projeto, quanto para auxiliar desenvolvedores que queiram
manipulá-lo.

Todo o código fonte do Lim está retido dentro do sub-diretório `./src/`, logo, ao falar
da *arquitetura do projeto*, é da organização desse sub-diretório que estamos falando.
Entretanto, antes de nos aprofundar-mos nele, é importante conhecer-mos um pouco sobre os
outros sub-diretórios, afinal, como dito anteriormente, seu conteúdo existe para facilitar
o entendimento e desenvolvimento desse projeto.

Vamos primeiro dar uma olhada no que há na raiz deste diretório:

```
.git/  .github/ bin/  docs/
.gitignore CMakeLists.txt
lim-icon.png LICENSE README.md

tests/ src/
```

Acredito que não devo me preocupar em explicar o primeiro grupo de elementos, visto que
ambos são quase que um padrão entre repositórios (*exceto `lim-icon.png`*), principalmente
entre aqueles hospedados no *GitHub*. Então vamos aproveitar essa brechar e cortar caminho.

> [!IMPORTANT]
> Para utilizar o `CMakeLists.txt`:
> * Mova-se para a raiz do projeto.
> * Crie um novo sub-diretório (*recomendo nomeá-lo como `build`*).
> * Execute o *Cmake* (*`cmake ..`*).

## Testes unários

Com o intuito de proteger o código e facilitar sua depuração, diversas partes do projeto
foram desenvolvidas para funcionar de maneira "independente", de modo a que possam ser
facilmente incorporadas em outros programas, seja para uso ou para depuração.

O sub-diretório `./tests/` é destinado a armazenar uma série de algoritmos
(*escritos em `Bash script`, `C` e `Lua`*), encarregados de explorar essa característica
para fins de depuração.

Antes de prosseguir, valor vislombrar o conteúdo desse diretório:

```
run
scripts/
   +-> args
   |    +-> args - files - main.c
   +-> header
   |    +-> args - files - main.c
   +-> queue
   |    +-> args - files - main.c
   +-> create-main
   +-> start-tests
   +-> debug-common.h
```

Como é possível notar, há um padrão entre os conteúdos presentes em: `args/`, `header/` e
`queue/` (*referidos aqui como "pacotes"*). Este foi adotado como forma de facilitar a
criação, manipulação e remoção de testes. Seu conteúdo é utilizado, por um algoritmo, para
compilar um executável destinado a depuração da lógica extraída do Lim. Vamos descobrir
para que esses arquivos de nome padronizado servem:

* `args`: lista de argumentos que serão utilizados nos testes. Geralmente é uma lista de
índices numéricos, de `0` a `n`.

* `files`: arquivos fonte do Lim que são necessários para a compilação do executável
de depuração. Geralmente envolve um arquivo principal e pequenas dependências.

* `main.c`: detém uma lógica especifica para alimentar e manipular dados dos algoritmos,
além de exibir suas saídas.

> [!NOTE]
> `debug-common.h` contém algumas macros "facilitadoras", utilizadas em `main.c`.

Agora que o intuito desse padrão foi compreendido, chegou o momento de entender como
o algoritmo responsável por manipulá-los funciona.

Este é formado por três *scripts*, escritos em *Bash*, sendo eles: `run`, `create-main` e
`start-tests`. Tal trio é encabeçado por `run`, que delega determinados valores aos
demais, para que os mesmos realizem sua função. Resumidamento, esta é a tarefa de cada um:

* `run`: capturar os argumentos que lhe foram passados e, baseado neles, definir quais
ações serão tomadas. Define variáveis, constantes e funções para uso posterior nos
*scripts* a seguir.

* `create-main`: copia o conteúdo de `main.c` para um arquivo temporário e o usa, junto
aos arquivos listados em `file`, para criar o executável de depuração. Caso um erro ocorra
durante a compilação, todo o processo será encerrado e o erro será exibido.

* `start-tests`: executa uma chamada do executável de testes para cada linha de argumentos
capturada em `args`, usando eles como argumento para tal programa. A cada chamada, exibe a
saída do executável. Por padrão, cada teste será executado apenas após a tecla *[enter]*
ser pressionada.

> [!IMPORTANT]
> Modificar, mover ou remover os arquivos temporários
> (*gerados por `run` em `./tests/script/.tmp/*`*), durante a execução de `start-tests`
> (*poderá*) causar erros.

Tal programa de depuração também possui seus próprios parâmetros, pensados para facilitar
ainda mais o processo de análise. Ambos encontram-se abaixo:

* *Nenhum*: exibe uma mensagem similar a esta: `./run < keyword | flag > [ flag ]`

* `keyword`: nome do *pacote* que será testado.

* `flag`: caractere, prefixados por `-`, que permite a execução de uma ação específica.
Estão disponíveis:
	
	* `-a`: executa os testes sem exibir sua saída e sem exigir que a tecla *[enter]* seja
	pressionada, a menos que um erro ocorra.

	* `-c`: gera o executável de testes na pasta `./tests/lim-broken`
	(*nomeado como `lim`*). Não realiza nenhum teste.
		
	* `-g`: executa os testes normalmente, com o acréscimo de gerar o executável de
	depuração (*assim como `-c`*) caso algum erro ocorra.
		
	* `-r`: deleta o executável de testes, caso ele exista, do contrário, nada é feito.
	Não realiza nenhum teste.

> [!TIP]
> O executável gerado com `-c` e `-g` é compatível para depuração com *GDB*.

## Arquivos fonte

Agora que o sub-diretório `./tests/` foi compreendido, chegou a hora de mergular-mos em
`./src`. Este importantíssimo sub-diretório é responsável por armazenar todo o código
fonte necessário para compilar o Lim.

> [!IMPORTANT]
> Neste trecho, aprofundarei-me apenas no padrão de arquitetura dado a `./src/`, que
> emgloba a maneira como os arquivos estão dispostos e como interagem entre si. Para
> informações sobre o funcionamento do código presente nos arquivos fonte, consulte:
> [Por trás dos panos](#pos-trás-dos-panos).

O modelo de arquitetura deste sub-repositório, nomeado como
*Padrão de Arquitetura em Pirâmide*, foi desenvolvido com o intuito cobrir os seguintes
pontos:

* Limitar o "alcance" de interação entre arquivos, de modo a evitar que arquivos "olhem
para trás".

* Separar trabalhos distintos em arquivos diferentes, nomeados de maneira a explicitar sua
tarefa.

* Organizar claramente a ordem em que ocorrem os processo.

> [!NOTE]
> Esse modelo é baseado nos padrões: *Layered* e *Master-slave*.

Como forma de facilitar seu entendimento, dividirei a explicação desse modelo com base nos
conceitos que o formam:

* "Isolamento" de dependências
* Separação e ordenação de processos

### Camadas

Durante o desenvolvimento de um programa, é comum que arquivos partilhem código uns com
os outros, sejam funções, variáveis ou semelhantes. Esta é uma funcionalidade
indispensável, mas que torna-se um pesadelo caso gerenciada de maneira grotesca.

Como forma de evitar o sugimento de uma grande teia de aranha, onde arquivos em `G/`,
dependem de arquivos em `C/`, que, por sua vez, dependem de arquivos em `L/`, o
conceito de "isolamente" foi empregado.

Este consiste em limitar a interação entre arquivos, de modo a que eles possam comunicar-se
apenas com outros que encontra-se na mesma pasta ou dentro de sub-pastas. Ou seja, um dado
arquivo não pode cominucar-se com outros posicionados anteriormente. Veja o exemplo:

```
> a.file B.dir/
>         |
>   c.file D.dir e.file
>            |
>          f.file
```

* `a` pode incluir o código de todo o conteúdo de `B/` e `D/`.
* `c` pode incluir o código de `e` e o código de todo o conteúdo de `D/`.
* `e` pode incluir o código de `c` e o código de todo o conteúdo de `D/`.
* `f` não podem incluir nenhum código externo.

> [!IMPORTANT]
> Obviamente, isso não se aplica aos cabeçalhos do *Padrão C*.

> [!TIP]
> Resumindo: não é permitido usar `..` no caminho dado a `#include`.

### Mestres

Uma peça chave fundamental durante o desenvolvimento de um projeto, é a ordenação de seus
processos. Sabendo disso, foi adotado, para este padrão, um conceito de organização que
baseia-se na fragmentação do trabalho e delegação de suas partes a diferentes arquivos,
os quais são controlados por um "arquivo mestre".

"Arquivo mestre", neste contexto, é o termo empregado a um arquivo encarregado de manipular
o conteúdo (*variáveis, funções, ...*) de outros, fornecendo e alterando valores, tratando
e validando retornos. Além de realizar pequenas operações.

Neste projeto, há três "arquivos mestre":

* `./src/main.c`: responsável por orquestrar todo o funcionamento do programa.
* `./src/args/process-master.c`: responsável pela verificação dos argumento passados ao
Lim, por meio do uso do conteúdo de `./src/args/*`.
* `./src/core/process-master.c`: reponsável por efetuar a compactação dos arquivos
passados ao Lim como argumento, por meio do uso do conteúdo de `./src/core/*`.

---

# Por trás dos panos
