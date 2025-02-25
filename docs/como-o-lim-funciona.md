# Como o Lim funciona?

---

* [Introdução](#introdução)
* [A arquitetura do projeto](#a-arquitetura-do-projeto)
	* [Testes unários](#testes-unários)
	* [Arquivos fonte](#arquivos-fonte)
		* [Camadas](#camadas)
		* [Mestres](#mestres)
* [Por trás dos panos](#por-trás-dos-panos)
	* [Cabeçalhos do *Cmake*](#cabeçalhos-do-cmake)
	* [O grande maestro](#o-grande-maestro)
	* [Verificando e validando argumentos](#verificando-e-validando-argumentos)
		* [Mensagens fatais](#mensagens-fatais)
		* [Bandeiras](#bandeiras)
		* [Mensagens de auxílio](#mensagens-de-auxílio)
		* [O processo em si](#o-processo-em-si)
			* [Buscando bandeiras informáticas](#buscando-bandeiras-informáticas)
			* [Buscando o arquivo de entrada](#buscando-o-arquivo-de-entrada)
			* [Verificando os demais argumentos](#verificando-os-demais-argumentos)
			* [Definindo valores omitidos](#definindo-valores-omitidos)
				* [Arquivo de destino](#arquivo-de-destino)
				* [Tabela de referência](#tabela-de-referência)
			* [Verificando o arquivo de saída](#verificando-o-arquivo-de-saída)
			* [Compartilhando valores](#compartilhando-valores)
	* [*Buffers*](#buffers)
		* [Arquivos temporários](#arquivos-temporários)
		* [Filas](#filas)
			* [Inicializando](#inicializando)
			* [Adicionando](#adicionando)
			* [Obtendo](#obtendo)
			* [Liberando](#liberando)
	* [Lendo o *header.lim*](#lendo-o-headerlim)
		* [Verificando o arquivo](#verificando-o-arquivo)
		* [Buscando as partições](#buscando-as-partições)
		* [Aprontando as saídas](#aprontando-as-saídas)
	* [O núcleo](#o-núcleo)
		* [As ferramentas](#as-ferramentas)
			* [A estrutura global](#a-estrutura-global)
			* [O tipo *Queue*](#o-tipo-queue)
			* [Arquivo opcional](#arquivo-opcional)
			* [Tratamento seguro para cadeias](#tratamente-seguro-para-cadeias)
			* [Modo verboso](#modo-verboso)
		* [O algoritmo principal](#o-algoritmo-principal)
			* [O cérebro](#o-cérebro)
			* [Verificando trechos](#verificando-trechos)
			* [Tratando fragmentos](#tratando-fragmentos)
			* [Escopos de ambientes](#escopos-de-ambientes)
			* [Apelidos](#apelidos)
			* [Linearmente falando](#linearmente-falando)
				* [Compactando o arquivo de entrada](#compactando-o-arquivo-de-entrada)
				* [Construindo o arquivo de saída](#construindo-o-arquivo-de-saída)
* [Considerações finais](#considerações-finais)

---

# Introdução

Todo bom programa precisa de uma documentação, principalmente quando seu propósito é tão
peculiar quanto o deste. Nesse artigo, pretendo explicar **todos** os processos do Lim:
como ocorrem, quando ocorrem, porque ocorrem e afins.

Esse conteúdo é destinado a todo e qualquer indivíduo que, por algum motivo, decida
estudar, modificar ou aprimorar o código deste projeto. Podendo também ser aproveitado
por usuários entusiastas que queiram mais informações sobre como as coisas ocorrem
por baixo dos panos.

---

# A arquitetura do projeto

Assim como qualquer outro diretório de projeto, esse não é formado apenas por arquivos
fonte, tendo em si uma série de outros arquivos auxiliares, que servem tanto para
facilitar a compilação do programa, quanto para facilitar seu desenvolvimento e
manipulação.

Todo o código fonte do Lim está retido dentro do subdiretório `./src/`, logo, ao falarmos
da *arquitetura do projeto*, é da organização desse subdiretório que estamos falando.
Entretanto, antes de nos aprofundarmos nele, é importante conhecermos um pouco sobre os
demais subdiretórios, afinal, como dito anteriormente, seu conteúdo existe para simplificar
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

## Testes unários

Com o intuito de proteger o código e facilitar sua depuração, diversas partes do projeto
foram desenvolvidas para funcionar de maneira "independente", de modo a que pudessem ser
facilmente incorporadas em outros programas, seja para uso ou para depuração.

O subdiretório `./tests/` é destinado a armazenar uma série de algoritmos, encarregados de
explorar essa característica para fins de depuração.

Antes de prosseguir, vamos vislumbrar o conteúdo desse subdiretório:

```
./tests/
    +-> run
    +-> scripts/
           +-> args/
           |    +-> args - files - main.c
           +-> header/
           |    +-> args - files - main.c
           +-> queue/
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

* `args`: lista de argumentos que serão utilizados nos testes, geralmente formada por
índices numéricos de `0` a `n`.

* `files`: arquivos fonte do Lim que são necessários para a compilação do executável
de depuração. Geralmente envolve um arquivo principal e "pequenas" dependências.

* `main.c`: detém uma lógica específica para alimentar, manipular e exibir as saídas dos
algoritmos.

> [!NOTE]
> `debug-common.h` contém algumas macros "facilitadoras", utilizadas por `main.c`.

Agora que o intuito desse padrão foi compreendido, chegou o momento de entendermos como
o algoritmo responsável por manipular tais arquivos funciona. Este é formado pelos
*scripts*: `run`, `create-main` e `start-tests`. Abaixo encontra-se o propósito de cada um:

* `run`: captura os argumentos que lhe foram passados e, baseado neles, define quais
ações serão tomadas. Declara variáveis, constantes e funções para uso posterior nos
*scripts* seguintes.

* `create-main`: copia o conteúdo de `main.c` para um arquivo temporário e o usa, junto
aos arquivos listados em `file`, para criar o executável de depuração. Caso um erro ocorra
durante a compilação, todo o processo será encerrado e o erro será exibido.

* `start-tests`: executa uma chamada do executável de testes para cada linha de argumentos
capturada em `args`, usando os valores capturados como argumento para tal programa. A cada
chamada, exibe a saída do executável. Por padrão, cada teste será executado **apenas** após
a tecla *[enter]* ser pressionada.

> [!IMPORTANT]
> Modificar, mover ou remover os arquivos temporários
> (*gerados por `run` em `./tests/script/.tmp/`*), durante a execução de `start-tests`
> *poderá* causar erros.

`run` possui seus próprios argumentos, os quais foram pensados para facilitar ainda mais o
processo de análise. Ambos encontram-se abaixo:

* *Nenhum*: exibe uma mensagem similar a esta: `./run < keyword | flag > [ flag ]`.

* `keyword`: nome do *pacote* que será testado.

* `flag`: caractere, prefixados por `-`, que permite a execução de uma ação específica.
Estão disponíveis:

	* `-A` executa **TODOS** os testes possíveis, com a mesma *configuração* de `-a`.

	* `-a`: executa os testes sem exibir sua saída e sem exigir que a tecla *[enter]* seja
	pressionada, a menos que um erro ocorra.

	* `-c`: gera o executável de testes na pasta `./tests/lim-broken/`
	(*nomeado como `lim`*). Não realiza nenhum teste.

	* `-g`: executa os testes normalmente, com o acréscimo de gerar o executável de
	depuração (*assim como `-c`*) caso algum erro ocorra.

	* `-r`: deleta o executável de testes, mas caso ele não exista, nada será feito. Não
	realiza nenhum teste.

> [!TIP]
> O executável gerado com `-c` e `-g` é compatível para depuração com *GDB*.

## Arquivos fonte

Agora que o subdiretório `./tests/` foi compreendido, chegou a hora de mergulharmos em
`./src/`. Este importantíssimo subdiretório é responsável por armazenar todo o código
fonte necessário para compilar o Lim.

> [!IMPORTANT]
> Neste trecho, irei aprofundar-me apenas no padrão de arquitetura dado a `./src/`, que
> engloba a maneira como os arquivos estão dispostos e como interagem entre si. Para
> informações sobre o funcionamento do código presente nos arquivos fonte, consulte:
> [Por trás dos panos](#por-trás-dos-panos).

Seu modelo de arquitetura, nomeado como *Padrão de Arquitetura em Pirâmide*, foi
desenvolvido com o intuito de cobrir os seguintes pontos:

* Limitar o "alcance" de interação entre arquivos, de modo a evitar que arquivos "olhem
para trás".

* Separar trabalhos distintos em arquivos diferentes, nomeados de maneira a explicitar sua
tarefa.

* Organizar claramente a ordem em que os processo ocorrem.

> [!NOTE]
> Esse modelo é baseado nos padrões: *Layered* e *Master-slave*.

Como forma de facilitar seu entendimento, dividirei a explicação desse modelo com base nos
conceitos que o formam:

* "Isolamento" de dependências.
* Separação e ordenação de processos.

### Camadas

Durante o desenvolvimento de um programa, é comum que arquivos partilhem código uns com
os outros, sejam funções, variáveis ou semelhantes. Esta é uma funcionalidade
indispensável, mas que torna-se um pesadelo caso gerenciada de maneira grotesca.

Como forma de evitar o surgimento de uma grande teia de aranha, o conceito de "isolamento"
foi empregado. Este consiste em limitar a interação entre arquivos, de modo a que eles
não possam comunicar-se com arquivos localizados/posicionados anteriormente. Veja o
exemplos:

```
a.file  B-dir/
      /   |   \
c.file D-dir/ e.file
         |
       f.file
```

* `a` pode incluir o código de todo o conteúdo de `B/` e `D/`.
* `c` pode incluir o código de `e` e o código de todo o conteúdo de `D/`.
* `e` pode incluir o código de `c` e o código de todo o conteúdo de `D/`.
* `f` não podem incluir nenhum código externo.

> [!IMPORTANT]
> Por mais que, teoricamente, seja possível acessar o conteúdo de um arquivo muitas
> pastas à frente (*como `A/B/C/D/E/a.file`*), isso não é recomendado. Limite-se a
> **no máximo** duas ou três pastas.

> [!TIP]
> Resumindo: não é permitido usar `..` no caminho dado a `#include`.

### Mestres

Uma peça chave fundamental durante o desenvolvimento de um projeto, é a ordenação de seus
processos. Sabendo disso, foi adotado, para este padrão, um conceito de organização que
baseia-se na fragmentação do trabalho e na delegação de suas partes a diferentes arquivos,
os quais são controlados por um "arquivo mestre".

"Arquivo mestre", neste contexto, é o termo empregado a um arquivo encarregado de manipular
o conteúdo (*variáveis, funções, ...*) de outros: fornecendo e alterando valores; tratando
e validando retornos. Além de realizar pequenas operações.

Neste projeto, há três "arquivos mestre":

* `./src/main.c`: responsável por orquestrar todo o funcionamento do programa, através da
inicialização de estruturas globais, chamada de "funções centrais" e afins.

* `./src/args/process-master.c`: responsável pela verificação dos argumentos passados ao
Lim, por meio do uso do conteúdo de `./src/args/`. Sua "função central" chama-se
`check_program_arguments`.

* `./src/core/process-master.c`: responsável por efetuar a compactação dos arquivos
passados ao Lim como argumento, por meio do uso do conteúdo de `./src/core/`. Sua "função
central" chama-se `read_source_file`.

---

# Por trás dos panos

Chegamos na parte mais entusiasmante da documentação, o momento em que o funcionamento do
código deste projeto será explicado, no caso, o conteúdo contido em `./src/`. Primeiro,
vamos dar uma olhada nesse subdiretório:

```
./src/
   +-> cmake-h/
   |      +-> version.h.in
   +-> main.c
   +-> args/
   |     +-> check.c/h
   |     +-> help.c/h
   |     +-> process-master.c/h
   |     +-> treat.c/h
   |     +-> fatal
   |     |     +-> error.h
   |     |     +-> info.h
   |     +-> flags
   |           +-> cmp.c/h
   |           +-> list.h
   +-> core/
         +-> check-content.c/h
         +-> nick-man.c/h
         +-> layer-env.c/h
         +-> process-master.c/h
         +-> treat.c/h
         +-> tools/
               +-> lim-global-variables.c/h
               +-> queue.c/h
               +-> read-header-lim.c/h
               +-> string-plus.c/h
               +-> verbose.c/h
```

Agora que adquirimos conhecimento sobre **todo** o conteúdo contido em `./src/`, chegou a
hora de navegarmos em seus subdiretório. Adentraremos essa aventura visando compreender o
funcionamento de seus algoritmos e, consequentemente, entender de que modo o Lim funciona.

## Cabeçalhos do *Cmake*

Vamos começar em ordem crescente: do mais fácil ao mais difícil. Baseado nessa ordem,
falaremos agora sobre o subdiretório `./src/cmake-h`.

Como é sabido por todos aqui (*creio eu*), o *Cmake* possibilita a criação de "arquivos
de entrada", que podem ser usados para gerar outros arquivos, os quais poderão possuir
informações contidas no `CMakeLists.txt`, como valores de variáveis.

Sabendo disso, esse diretório foi criado para armazenar, tanto os "arquivos de entrada"
(*criados por nós, desenvolvedores*), quanto os "arquivos de saída"
(*criados pelo Cmake; e ignorados pelo Git*).

> [!NOTE]
> Use a extensão `.h.in` em "arquivos de entrada" e `.h` para "arquivos de saída".

## O grande maestro

Assim como todo projeto escrito em *C*, este também possui uma função `main`, localizada no
"arquivo mestre" `./src/main.c` (*sendo a única presente nele*). Seu código é mínimo, mas
seu trabalho é de extrema importância, consistindo em:

1. Capturar os argumentos do programa.
2. Inicializar os membros de estruturas globais.
3. Adicionar funções de "liberação" à pilha da função `atexit`.
4. Fazer a *comunicação* entre diferentes algoritmos.
5. Chamar "funções centrais", as quais, majoritariamente, originam-se de outros "mestres".

## Verificando e validando argumentos

Chegou o momento de tratar de um algoritmo mais complexo. Falaremos agora sobre a lógica
responsável por verificar e validar os argumentos passados ao Lim, a qual localiza-se em
`./src/args/`.

É de grande importância que obtenhamos conhecimento sobre todo o contexto de seu interior
para que tornemo-nos capazes de compreender seu algoritmo como um todo, por conta disso,
navegaremos por todos os arquivos contidos em `./src/args/` antes de abordar o
funcionamento de seu algoritmo.

### Mensagens fatais

Começaremos por uma pasta de nome peculiar: `./src/args/fatal/`. Seu conteúdo consiste em
arquivos de cabeçalho, formados por macros padronizadas, onde ambas:

* Imprimem uma mensagem diferente.
* Encerram o programa.

Elas estão divididas em dois grupos:

* `error.h`: encerra o programa após um dado erro, durante a verificação dos argumentos.
Cada macro possui seu próprio código de erro. Veja:
[Códigos de erro](https://github.com/duckafire/lim/blob/main/docs/codigos-de-erro.md).

* `info.h`: encerra o programa após uma *bandeira informativa* ser encontrada. Todas as
macros encerram o programa com sucesso (*`0`*). Veja:
[Bandeiras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md).

A estruturas de tais macros **assemelha-se** a seguinte:

``` c
#define ERROR(code, str) \
	puts(str "\n"); \
	exit(code)

#define ERROR_foo \
	ERROR(1, "foo")
```

### Bandeiras

Chegou o momento de adentrarmos em `./src/args/flags/` e, como é de se imaginar, seu
conteúdo é voltado às bandeiras do Lim.

> [!IMPORTANT]
> Para informações especificas sobres as bandeiras (*como: usos, tipos e afins*), consulte:
> [Bandeiras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md)

Seu interior é compostos por alguns poucos arquivos, responsáveis pela:

* `./src/args/flags/list.h`: Declaração das bandeiras, no formato de constantes de
pré-processamento (*ambas prefixadas por `FLAG_`*).

* `./src/args/flags/cmp.c/h`: Declaração da "função de comparação" (*`flag_cmp`*).

Como sabemos, Lim possui duas versões (*"curta" e "longa"*) para todas as suas bandeiras,
logo, faz-se necessário declarar uma constante para cada, entretanto, visto que as
"representações gráficas" delas servem apenas para realizar comparações, isso pôde ser
facilmente evitado através de uma "declaração conjunta", ou seja, por meio da atribuição
de ambas as versões de uma bandeira a uma única constante, da seguinte maneira:

``` c
#define FLAG_FOO "-f", "--foo"
```

> [!IMPORTANT]
> Essa abordagem impossibilita algumas coisas, como a atribuição desses valores a
> variáveis, uso deles como argumento para macros e afins. Porém, como citado
> anteriormente, tais constantes não são utilizadas para nada além de comparações, logo não
> há problema algum no uso dessa abordagem.

Para realizar as comparações, entre os argumento e as bandeiras, foi desenvolvida a
seguinte função:

``` c
bool flag_cmp(char *arg, char *f, char *flag){
	return (strcmp(arg, f) == 0 || strcmp(arg, flag) == 0);
}
```

> [!TIP]
> Observe esse exemplo de uso:
>
> ``` c
> if( flag_cmp("arg-foo", FLAG_FOO) )
> 	puts("Foo");
> ```

Dito isso, podemos concluir que:

* As "representações gráficas" das bandeiras são usadas apenas em comparações
(*entre elas e uma dado argumento*).

* Ambas as versões de uma mesma bandeira são detidas dentro da mesma constante de
pré-processamento.

* As constantes, que detêm as bandeiras, são utilizadas apenas como argumento para a função
`flag_cmp`.

> [!IMPORTANT]
> Vale ressaltar que, além das bandeiras do Lim, constantes para palavras-chave de
> bandeiras também são definidas em `flags/list.h`, como é o caso de (*`l`*) `list`,
> palavra-chave da bandeira (*`-h`*) `--help`. Ambas seguem o mesmo padrão de "declaração
> conjunta". Elas são prefixadas por `<FLAG-NAME> + _ARG_ + <ARG_NAME>`.

### Mensagens de auxílio

Partindo agora para o conteúdo raiz de `./src/args/`, falaremos brevemente sobre o que está
contido em `help.c/h`, que, como é de se imaginar, é voltado à ajuda
(*durante a execução do programa*).

> [!TIP]
> Tais informações podem ser acessadas por meio da utilização da bandeira (*`-h`*)
> `--help`. Para obter mais informações sobre acesse:
> [Bandeiras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md).

As **funções** presentes nesses arquivos seguem o mesmo conceito das macros presentes em
`./src/args/flags/info.h`, sendo eles:

* Exibir uma mensagem.
* Encerrar o programa com sucesso.

Com o acréscimo de verificarem o conteúdo recebido.

> [!NOTE]
> Por conta da arquitetura desse projeto, `help.c/h` não pode ser posicionado dentro de
> `./src/args/fatal/`.

### O processo em si

Enfim, chegou o momento de entendermos esse algoritmo. Primeiro vamos conhecer sua
estrutura globais:

``` c
struct Args_Flags_And_Status{
	unsigned int index;
	char **value;

	bool content_shared;
	char *program_version;

	struct{
		char *source;
		char *destine;
	}files_name;
	
	struct{
		bool verbose;
		bool replace;
		bool header_file;
		char *lib_name;
	}flags;

};

extern struct Args_Flags_And_Status args;
```

* `index`: último índice do vetor de argumentos (*`argv`*).
* `**value`: ponteiro para o vetor de argumentos.
* `content_version`: cadeia de caracteres contida em `LIM_VERSION`.
* `content_shared`: indica se os valores armazenados nesta estrutura foram compartilhados
com a estrutura global de `./src/core/`.
* `files_name.*`: nome dos arquivos fonte/entrada e destino/saída atribuídos ao Lim.
* `flags.*`: estado das bandeiras (*ativada/desativada; conteúdos*).

> [!NOTE]
> Se `args.content_shared` for verdadeiro, a função `void args_free_env(void)` terá seu
> efeito "anulado", pois a custodia de seus endereços dinâmicos será dada à estrutura
> global de `./src/core/`.

Os processos desse algoritmo iniciam-se em `./src/main.c`, após a inicialização de sua
estrutura global, com a chamada da função `void check_program_arguments(void)`.

``` c
int main(int argc, char *argv[]){
	args_init_env(argc, argv, LIM_VERSION);
	atexit(args_free_env);

	check_program_arguments();
```

`check_program_arguments`, função declarada em `./src/args/process-master.c/h`, é
responsável por efetuar chamadas às funções presentes em `./src/args/check.c/h` e
`./src/args/treat.c/h`.

#### Buscando bandeiras informáticas

A primeiro coisa que `check_program_arguments` faz é chamar a função de verificação
`void is_it_information_flag(void)`. Tal função é responsável por verificar o primeiro
argumentos passado ao Lim, onde:

1. Se não houver nenhum argumento, uma mensagem de padrão será exibida.
2. Caso uma *bandeia informativa* seja encontrada, sua respectiva mensagem será exibida.
3. Do contrário, o fluxo do programa prosseguirá.

> [!NOTE]
> Argumentos excedentes àqueles requeridos pelas *bandeiras informativas* serão ignorados.

#### Buscando o arquivo de entrada

Caso o programa não seja interrompido na etapa anterior, será entendido que a intensão do
usuário é compactar um arquivo. Então o fluxo do programa retornará a função
`check_program_arguments`, que chamará a função de verificação
`void search_and_set_source_file(void)`.

O objetivo de `search_and_set_source_file` é checar se o primeiro argumento passado ao Lim
é o nome de uma arquivo válido.

> [!IMPORTANT]
> Ou seja, caso o primeiro argumento dado ao Lim não seja uma *bandeira informativa*,
> ele **deverá** ser um nome de arquivo.

Para efetuar esse verificação, os seguintes pontos são checados:

1. O argumento inicia com `'-'`.
2. O arquivo não existe.
3. O arquivo não possui a extensão `.lua`.

Caso algumas dessas afirmações seja verdadeira, um respectivo erro fatal ocorrerá.

> [!WARNING]
> O nome do arquivo de origem não deve começar com '-'.

> [!NOTE]
> Em meio a esses processos, o membro `args.files_name.source` receberá o primeiro 
> argumento passado ao Lim, que será usado para abrir o arquivo entrada (*para leitura*) em 
> processos posteriores.

#### Verificando os demais argumentos

Agora que o primeiro arquivo foi tido como sendo um nome de arquivo válido, voltaremos
a `check_program_arguments`, que, dessa vez, chamará a função de verificação
`void read_other_arguments(void)`.

`read_other_arguments` foi criada para verificar os demais argumentos que foram passados
ao Lim, para isso, primeiro, ela verifica se há mais algum argumento além do primeiro, em
caso negativo, ela se interromperá e fazendo o fluxo retornar a `check_program_arguments`,
em caso positivo, ele fará o seguinte:

1. Iniciará um *loop* para ler os argumentos (*partindo do segundo*).
2. Comparará cada um dos argumentos restantes com todas as bandeiras do programa.

Um erro fatal ocorrerá se qualquer argumento for:

* Igual a uma *bandeira informativa*.
* Uma *bandeira de ação* que **exija** um sufixo, mas que não possua um.
* Uma bandeira já utilizada na chamada (*repetida*).
* Uma bandeira inválida.
* Ou um argumento inválido (*qualquer valor "estranho"*).

> [!NOTE]
> Sempre que uma bandeira válida for encontrada, seu membro respectivo na estrutura `args`
> terá seu valor alterado.

#### Definindo valores omitidos

Como sabemos, a especificação do nome (*e caminho*) do arquivo de saída e o nome da tabela
de referência (*para a biblioteca*) são argumentos opcionais, entretanto eles são valores
indispensáveis para os processos do Lim, por conta disso, faz-se necessário que tal
programa defina um "valor padrão" para esses campos.

Sabendo disso, após a chamada da função de verificação `read_other_arguments`,
(*em `check_program_arguments`*), é verificado se tais valores foram especificados e
corretamente salvos pelo processo anterior, caso não, algumas funções serão chamadas para
executar um tratamento padrão.

##### Arquivo de destino

Em um cenário no qual o arquivo de saída tenha seu nome e caminho omitidos,
`check_program_arguments` chamará a função de tratamento
`void set_destine_file_name(const char *name)`, usando `args.files_name.source` como
argumento.

Tal função copiará o conteúdo de `args.files_name.source` para um endereço dinâmico,
substituirá sua extensão por `.lim` e atribuirá tal endereço a `args.files_name.destine`.
Ou seja, `set_destine_file_name` definirá o nome do arquivo de saída como sendo igual
ao nome do arquivo de entrada, além de definir que ele deve ser posto no mesmo
diretório em que o arquivo de entrada está.

##### Tabela de referência

Seguindo a linha de raciocínio anterior, temos um cenário no qual, dessa vez, o
identificador (*vulgo nome*) da tabela de referência à biblioteca foi omitido, nesse caso,
`check_program_arguments` chamará a função de tratamento
`void set_lib_name(const char *name)`, usando (*também*) `args.files_name.source` como
argumento.

O algoritmo presente em `set_lib_name` efetua os seguintes processos:

1. O conteúdo de seu argumento é copiado para um endereço dinâmico.
2. Caso o conteúdo possua a extensão `.lua`, a mesma será removida.
3. Se houver um caminho embutido no conteúdo, tal caminho será removido.
4. **Se**:
	* O resultado desse processo for uma cadeia vazia, `'_'` será atribuído ao primeiro
	índice da memória em questão.
	* **Do contrário**, os caracteres restantes serão analisados da seguinte maneira:
		* Caso o primeiro caractere não seja um alfanumérico (*ou igual a `_`*), o mesmo
		será substituído por `'_'`.
		* Quanto aos demais, eles serão substituídos por `'_'` caso forem iguais a qualquer
		caractere especial a diferente de `'_'`.
5. Ao fim do processo, o membro `args.flags.lib_name` receberá a cadeia resultante.

> [!NOTE]
> Mesmo identificadores especificados são submetidos a tal validação.

#### Verificando o arquivo de saída

Após todos esses processos, caso nenhum erro ou finalização ocorra, o fluxo retornará
novamente a `check_program_arguments`, que chamará sua última função:
`void does_dest_file_already_exist(void)`.

Esta função de verificação é responsável por checar se o arquivo de saída já existe.
Em caso positivo, será verificado (*por meio do membro `args.flags.replace`*) se a bandeira
(`-r`) `--replace` foi utilizada, caso não, um erro fatal ocorrerá
(*informando que o arquivo já existe*), do contrário, o programa seguirá seu fluxo.

Caso a chamada sobreviva a mais essa verificação, a função `check_program_arguments` se
encerrará, fazendo o fluxo voltar para a função `main`.

#### Compartilhando valores

De volta a `./src/main.c`, podemos observar que uma importante tarefa está prestes a ser
executada: as informações principais armazenadas na estrutura global de `./src/args/` terão
sua custodia dada à estrutura global de `./src/core/`.

Junto a esse conjunto de atribuições, o valor de `args.content_shared` será atualizado
para `true`, assim desabilitando a função de limpeza responsável por liberar a memória
alocada para a estrutura de `./src/args/`.

## *Buffers*

Chegamos em um ponto interessante, pois a partir de agora, os sistemas voltados ao
armazenamento da saída dos algoritmos se tornará mais complexo, dado que suas saídas
se tornarão incomparavelmente mais numerosas do que aquelas geradas pelo algoritmo
anterior, de modo a que variáveis e vetores tornem-se insuficientes para tal tarefa.

Tais dados podem ser divididos em dois tipos:

1. Valores extremamente volumosos e imprevisíveis, que são (*opcionalmente*) formatados/
filtrados de modo simples (*antes de sua gravação na memória*). Exigidos apenas uma vez.

2. Dados majoritariamente curtos, mas numerosos, que necessitam ser capturados e isolados
uns dos outros. São constantemente lido e requeridos.

Visto que a solução empregada a esses problemas é muito popular nos algoritmos seguintes,
faremos uma pausa na explicação linear que estava ocorrendo e adentraremos nesse
importantíssimo assunto.

### Arquivos temporários

Grande parte das operações empregadas pelos algoritmos posteriores estarão ligadas à
leitura de arquivos persistentes, onde as informações obtidas são, muitas vezes, submetidas
a filtros e, só então, salvas no *buffer*.

Sabendo disso, organizei alguns pontos relacionados com tais dados:

* Serão salvos linearmente, ou seja, na ordem em que foram capturados.
* Não precisarão ser editadas após sua escrita.
* Nenhum algoritmo exigirá trechos de seu conteúdo.
* Serão exigidos apenas uma vez e como um todo.
* Exigirão um espaço dinâmico para que possam ser armazenados.

Tendo isso em mente, pude concluir que a melhor forma de armazenar dados com estas
características seria por meio de *arquivos temporários*, pois:

* Eles expandem-se dinamicamente.
* Possuem uma *API* considerável em `stdio.h`.
* São seguros quanto à memória.

> [!TIP]
> É possível criar arquivos temporários usando a função `FILE* tmpfile(void)`.

Mas: *por que não armazenar essas informações diretamente no destino?*

> A resposta é simples: certos tipos de dados tem prioridade sobre outros, ou seja, eles
> precisam ser escritos primeiros no arquivo de saída.

### Filas

Agora que um dos "problemas" havia sido resolvido, bastava buscar uma solução para o outro.
Este era um pouco mais complexo, já que seus dados precisariam ser gravados, lidos e
obtidos imprevisíveis vezes, logo seu meio de armazenamento deveria ser rápido, expansível
e flexível (*para que pudesse ser empregado em múltiplas situações*).

Baseado nisso, bastava encontrar a melhor estrutura de dados para que tal problema
desaparecesse. Após algumas ideias, cheguei a conclusão de que **filas** seriam a melhor
escolha, por conta de sua flexibilidade, lógica simples e maleabilidade.

> [!NOTE]
> Uma estrutura de *Árvore Binária AVL* foi desenvolvida como forma de obter mais
> velocidades em cenários específicos, entretanto não foi possível atingir esse objetivo,
> por conta disso, ela foi removida no *commit*:
>
> ```
> commit b3a7cdcfb0d8cefa8e989f31f1cfffce65f32789
> Author: DuckAfire <155199080+duckafire@users.noreply.github.com>
> Date:   Tue Jan 21 18:19:43 2025 -0300
>
> Rework in directory
>
> .gitignore     | 1 +
> CMakeLists.txt | 30 +++++++-------
> ...
> ```

Visto que a solução já estava em mãos, fazia-se necessários desenvolver uma estrutura para
os itens, além de uma "mini *API*" para realizar operações com tais tipos compostos.

Sabendo disso, a seguinte estrutura foi construída:

``` c
typedef struct Queue{
	char *ident;
	char *table_key;
	char *nick;
	unsigned short quantity;
	bool is_const;
	struct Queue *next;
}Queue;
```

* Conteúdos: valores que serão armazenados.
	* **`ident`**: **conteúdo principal**, logo **nunca será `NULL`**. Seu valor depende do
	contexto, mas, geralmente, é o identificador de uma variável, tabela ou função extraída
	do arquivo de entrada.
	* `table_key`: para o caso de `ident` ser o identificador de uma tabela *em uso*,
	tal vetor armazenará todo o conteúdo após `.` ou `:` (*`foo.x -> .x`*), do contrário
	armazenará `NULL`.
	* `nick`: "versão" compactada de `ident` ou `NULL`, caso o contexto não exija
	compactação.
* Utilitários: usados pela "mini *API*" para realizar operações.
	* `quantity`: indica quantas vezes ocorreu a tentativa de reinserir `ident` em uma
	mesma fila (*o valor inicial é `0`*).
	* `is_const`: indica que `ident` não deve ser compactado. Se for `true`, `nick` será,
	obrigatoriamente, `NULL`.
* Membro da fila: partes fundamentais de uma fila.
	* `next`: aponta para o próximo item da fila.

#### Inicializando

Agora que entendemos a estrutura dos itens de uma fila, vamos usar esse conhecimento para
compreender o funcionamento da "mini *API*" que está armazenada em
`./src/core/tools/queue.c/h`, a qual é destinada a abstrair e automatizar os procedimentos
envolvendo filas.

A primeira função que iremos ver é a
`Queue* qee_create(char *nick, char *ident, char *table_key, bool is_const)`, que, como
seu nome dá a entender, é encarregada de criar um item de fila, além de inicializar seus
membros, tanto com os argumentos que lhe foram passados, quanto com valores padrão.

``` c
Queue* qee_create(char *ident, char *table_key, char *nick, bool is_const){
	new_item = malloc(sizeof(Queue));

	new_item->ident     = string_copy(ident);
	new_item->table_key = string_copy(table_key);
	new_item->nick      = string_copy(nick);
	new_item->quantity  = 0;
	new_item->is_const  = is_const;
	new_item->next      = NULL;

	return new_item;
}
```

> [!WARNING]
> `new_item` é um ponteiro global estático, presente em `./src/core/tools/queue.c/h`, mas
> ele não é o único, havendo também:
>
> ``` c
> static bool qee_add_item_return;
> static bool qee_add_item_quant_upped;
> static Queue *new_item;
> static Queue *tmp_item;
> static bool bigger_to_lower_is_allow = true;
> static IS_DUPLIC treat_duplicated_item;
> ```
> Já `IS_DUPLIC` é um tipo baseado no seguinte `enum`:
>
> ``` c
> typedef enum{
> 	QEE_DROP,
> 	QEE_UP_QUANT,
> 	QEE_INSERT,
> }IS_DUPLIC;
> ```
>
> Onde suas macros indicam de que modo itens duplicados devem ser tratados,
> respectivamente:
>
> * `QEE_DROP`: devem ser descartados.
>
> * `QEE_UP_QUANT`: devem ser descartados e suas "versões" já presentes na fila devem ter
> seu membro `quantity` incrementado em `+1`.
>
> * `QEE_INSERT`: devem ser adicionados a fila (*após os similares já presentes na mesma.*)
> Não altera a quantidade de nenhum item.

#### Adicionando

Dando prosseguimento, falaremos agora sobre a função
`bool qee_add_item(Queue **head, char *nick, char *ident, char *table_key, bool is_const, IS_DUPLIC treat)`
e suas subordinadas: `static Queue* insert_item(Queue *item)` e
`static Queue* ordenate_queue(Queue *item)`.

Esse conjunto de funções é responsável por duas coisas:

1. Adicionar itens a uma dada fila.
2. Opcionalmente, manter sua ordenação.

Partindo do início, vamos entender tal grupo de funções:

1. Tudo inicia-se após a chamada de `qee_add_item`, onde os valores das seguintes variáveis
e ponteiros serão definidos:
	* `new_item = qee_create(ident, table_key, is_const)`: *possivelmente* será
	adicionado à fila.
	* `treat_duplicated_item = treat;`: define como itens duplicados serão tratados
	(*o valor de `ident` é usado para definir quais valores são duplicados*).
	* `qee_add_item_return = false;`: valor que será retornado por `qee_add_item`.
	* `qee_add_item_quant_upped = false;`: indica se a quantidade de algum item da fila foi
	atualizada.

2. Então, a função `insert_item` é chamada (*`*head = insert_node(*head)`;*
*seu algoritmo é simples, mas o fato de ser recursivo pode dar um nó na cabeça*):
	
	1. Ela, primeiramente, verificará se seu argumento é `NULL`, retornando `new_item` caso
	verdadeiro.

	2. Após a condição anterior falhar, o valor de `bigger_to_lower_is_allow` será
	verificado, caso verdadeiro, o caso seguinte da condição será verificado, nele ocorrerá
	uma comparação entre os membros `quantity` e o comprimento dos membros `ident` do
	*item atual* e de `new_item`, que, se dada como verdadeira, resultará na inserção do
	*novo item* entre dois itens já existentes.

	3. Seguido de uma nova falha condicional, o conteúdo presente no membro `ident`, do
	*item atual* e de `new_item`, será comparado, onde, se ambos forem iguais, um devido
	tratamento será empregado a `new_item`, baseado no valor de `treat_duplicated_item`.

	4. Por fim, caso nenhum processo anterior finalize a função, a mesma chamará a si,
	usando o próximo item como argumento e receptor de retorno, a modo a repetir todo esse
	procedimento, até que `new_item` seja adicionado á fila ou descartado. Após tal chamada
	recursiva, a função retornará seu próprio argumento.

3. Com o fim do trabalho de `insert_item`, em `qee_add_item`, será verificado se
`bigger_to_lower_is_allow` e `qee_add_item_quant_upped` são verdadeiros, onde, em caso
positivo, a função de ordenação `ordenate_queue` será chamado. Seu objetivo é garantir que
os itens presente na fila permaneçam ordenados do maior ao menor.

	1. Inicialmente, `ordenate_queue` verificará se o *item atual* é `NULL`, caso não,
	verificará se o *próximo item* é `NULL`. Qualquer condição destas que se provar
	verdadeira resultará no retorno do *item atual*.

	2. Após a falha da condição citada acima, ocorrerá uma comparação entre os membros
	`quantity` e o comprimento dos membros `ident` do *item atual* e do *próximo item*,
	que, se dada como verdadeira, resultará em um troca de posições, onde o
	*próximo item* tomará o lugar do *item atual*. Seguidamente, o atual "novo
	*item atual*" receberá o retorno uma chamada recursiva à `ordenate_queue`, que terá o
	"novo *próximo item*" como argumento.

	3. Ao final, caso a função não tenha sido finalizada anteriormente, `ordenate_queue`
	será chamada recursivamente, com o *próximo item* como argumento e receptor de seu
	retorno, o que desencadeará a repetição do processo acima, até que todos os valores
	sejam estejam ordenados corretamente. Depois dessa chamada, `ordenate_queue` retornará
	seu próprio argumento.

4. No final de `qee_add_item`, a mesma retornará `qee_add_item_return`, para indicar se
o novo item foi inserido na fila em questão.

> [!WARNING]
> O valor de `bigger_to_lower_is_allow` pode ser definido através do uso da função
> `void qee_bigger_to_lower(bool allow)`. Esta variável é responsável por especificar se
> a ordenação dos itens deve ser efetuada.
>
> Por padrão, os itens presentes em uma fila são ordenados daqueles com maior quantidade
> àqueles com menor, onde itens de mesma quantidade tem o comprimento de seu membro `ident`
> comparado para definir qual deles deverá ser posto primeiro. Se ambos os valores forem
> igual, o *novo item* será posto atrás do item atual, a menos que este padrão se repita
> para eles, nesse caso, tal procedimento será repetido até que o novo item seja
> devidamente posicionado.
> 
> Observe o exemplo abaixo:
> 
> ```
> +---+
> |cpp| [add]
> |x0 |
> +---+
> 
> +---+   +---+
> |lua| > |js |
> |x0 |   |x0 |
> +---+   +---+
> ```
> 
> ```
> +---+   +---+   +---+
> |lua| > |cpp| > |js |
> |x0 |   |x0 |   |x0 |
> +---+   +---+   +---+
> ```

> [!IMPORTANT]
> O valor de `qee_add_item_return` se tornará verdadeiro sempre que um item for introduzido
> à fila.

> [!NOTE]
> Após `insert_item` alterar a quantidade de um dado item, surgem grandes chances de sua
> nova quantidade torná-lo um item desordenado, pois `insert_item` não o realocará, caso
> isso seja necessário, já que suas habilidades para tal resumem-se ao momento da adição
> dele à fila.
>
> Para contornar esse problema, foi criada a função `ordenate_queue`, executada após
> `insert_item`, dentro de `qee_add_item`, caso `bigger_to_lower_is_allow` seja verdadeiro.

#### Obtendo

Partindo agora para um algoritmo mais simples, falaremos sobre a função
`Queue* qee_get_item(Queue *item, char *ident)`. Esta função recursiva trabalha sozinha
para obter um dado item de uma determinada fila.

Sua lógica consiste em três pontos:

1. Validação: se o item atual for igual a `NULL`, a função retornará `NULL`.
2. Comparação: se o conteúdo do seu segundo argumento for igual ao conteúdo do membro
`ident` do *item atual*, a função retornará o *item atual*.
3. Avanço: a função retornará uma chamada recursiva, que utilizará o *próximo item* como
primeiro argumento (*o segundo será mantido*).

#### Liberando

Como último membro dessa "mini *API*", temos as funções `void qee_free_queue(Queue *item)`
e `static void free_item(Queue *item)`, que, como é possível imaginar, são encarregadas de
liberar a memória alocada para os itens de uma fila.

Depois que o primeiro item de uma dada fila é dado como argumento à `qee_free_queue`, tal
função executará uma série de chamadas recursivas, que resultarão na liberação das memórias
alocadas para `ident`, `table_key`, `nick` e para a própria estrutura.

> [!NOTE]
> `free_item` existe apenas por conta que seu código é utilizado em duas funções, sendo
> elas: `qee_add_item` e `qee_free_queue`.

## Lendo o *header.lim*

Agora que o fluxo retorna à `main`, ela estará prestes a executar um processo de grande
importância: a verificação do *header.lim*.

> [!IMPORTANT]
> Esse tópico visa explica o funcionamento interno do algoritmo encarregado de ler e tratar
> o conteúdo e o próprio *header.lim*, para uma explicação relacionada aos seus usos,
> funcionalidades e afins, consulte:
> [O "header.lim"](https://github.com/duckafire/lim/blob/main/docs/o-header-lim.md)

Tal tarefa foi dada a função `HF_OUT_STATUS read_header_file(char **indiv_part_status)`,
armazenada em `./src/core/tools/read-header-lim.c/h`. Ela executa uma série de
verificações, que, se dadas como falsas, acarretam na leitura do conteúdo do *header.lim*.

> [!IMPORTANT]
> `HF_OUT_STATUS` é um tipo baseado no seguinte `enum`:
>
> ``` c
> typedef enum{
> 	HF_READING_DISABLE,
> 	HF_NOT_FOUND,
>	HF_IS_EMPTY,
> 	HF_CONTENT_READED,
> }HF_OUT_STATUS;
> ```

Antes de adentrarmos `read_header_file`, é importante que tenhamos ciência sobre uma coisa:
tanto o processo de leitura do *header.lim* (*como um todo*), quanto a leitura de suas
partições (*individualmente*) geram um "estado de saída" distinto.

Sabendo isso, antes de chamar `read_header_file`, a função `main` declara o seguinte:

``` c
char *part_status = "0000";
HF_OUT_STATUS file_status;
```

Onde ambos são responsáveis por:

* `part_status`: capturar o código de estado de saída de **todas** as partições do
*header.lim*. Para que tal tarefa seja concluída, seu endereço será dado como argumento
para `read_header_file`.

* `file_status`: capturar o código de estado de saída do *header.lim*, que indica o que
ocorreu durante sua verificação. Esse valor será retornado por `read_header_file`.

Agora que você já entende o propósito dos "periféricos" que acompanham a chamada da função
`read_header_file` (*na `main`*), vamos mergulhar em seu interior, visando compreender de
que modo seu algoritmo verifica, trata e lê o *header.lim*.

### Verificando o arquivo

Já dentro do `read_header_file`, a primeira coisa que podemos ver são três condições:

``` c
if(!lim.flags.header_file)
	return HF_READING_DISABLE;

lim.files.header_lim = fopen("header.lim", "r");
if(lim.files.header_lim == NULL)
	return HF_NOT_FOUND;
	
if(fgetc(lim.files.header_lim) == EOF)
	return HF_IS_EMPTY;
FSEEK;
```

> [!NOTE]
> `FSEEK` é uma *macro* ***local*** que contém `fseek(lim.files.header_lim, -1, SEEK_CUR)`,
> logo, nesse contexto, o intuito de seu uso é reverter a ação realizada pela função
> `fgetc`, chamada na última condição.

Como é possível imaginar, esse pequeno trecho é responsável por verificar, respectivamente:

1. Se a leitura do *header.lim* foi bloqueada.
2. Se o *header.lim* não existe.
3. Se o *header.lim* está vazio.

Caso ambas falhem, isso acarretará, inevitavelmente, no retorno de `HF_CONTENT_READED`, que
encontra-se ao fim de `read_header_file`.

### Buscando as partições

Após tais condições, um vetor local, do tipo `HF_READ_STATUS`, será declarado, o qual será
utilizado como argumento para uma chamada da função
`static void start_reading(HF_READ_STATUS *refined_status)`, que ocorrerá em seguida. Tal
função utilizará seu argumento para armazenar o estado de saída de todas as partições do
*header.lim*.

Ao adentrarmos em `start_reading`, podemos notar que o estado das partições é dividido em
dois: *bruto* e *refinado*. Onde:

* Estado bruto (*`bool`*): caso verdadeiro, indica que a próxima partição foi encontrada,
caso falso, indica que o final de *header.lim* foi encontrado. Cada partição possui seu
próprio "estado bruto".

* Estado refinado (*`int`*): é o estado de saída de cada partição, obtido através de um
processo que consiste na análise do "estado bruto" junto ao estado do *buffer*
(*vazio ou não vazio*) de uma dada partição.

Os "estados brutos" são preenchidos pelo retorno das funções de leitura abaixo:

* `static bool read_top_header(void)`
* `static bool read_code_scope(void)`
* `static bool read_list(Queue **buf, bool is_this_found)`

> [!NOTE]
> `read_list` é utilizada para ler *`funct list`* e *`table list`*.

Seguidamente, a função
`static HF_READ_STATUS refine_brute_status(FILE **buf, Queue **list, bool new_part_found, bool isfile)`
é utilizada para *refinar* os "estados brutos". Seu retorno é armazenado nos índices do
argumento de `start_reading`.

### Aprontando as saídas

Depois que `start_reading` preenche os índices de seu argumento, o fluxo do programa retorna
a `read_header_file`, lá o conteúdo de tais índices será convertido (*de `int`*)
para `char` e será armazenado no endereço apontado pelo argumento de `read_header_file`, o
qual receberá um espaço em memória justo, destinado a tal finalidade.

Por fim, `read_header_file` retornará `HF_CONTENT_READED`, indicando que o *header.lim* foi
lido com sucesso.

## O núcleo

Após o retorno do fluxo à `main`, esta se encarregará de iniciar o algoritmo responsável
por ler e compactar o arquivo de entrada, além de construir o arquivo de saída, que está
contido no subdiretório `./src/core/`.

Antes de tentarmos entender o funcionamento conjunto de tal algoritmo, é de grande
importância que compreendamos um coisa: que tipo de "funções auxiliares" são utilizadas
pelo "algoritmo principal".

### As ferramentas

Ao contrário do que se possa imaginar, `./src/core/` não detém apenas os arquivos que
formam o algoritmo de compactação do arquivo de entrado e o algoritmo de construção do
arquivo de saída, em seu interior há uma pasta chamada `./src/core/tools/`, a qual é
encarregada de armazenar uma série de pequenos algoritmos, criados para facilitar o
desenvolvimento dos processos principais.

#### A estrutura global

Assim como `./src/args/`, `./src/core/` também possui sua própria estrutura global,
que está disponível nos arquivos `./src/core/tools/lim-global-variables.c/h`. Esta
encontra-se abaixo:

``` c
typedef struct Local_Env{
	FILE *content;
	char *save_local_ident, *save_parameter, *save_for_loop;
	Queue *var, *func, *special;
	Local_Env_Type type;
	struct Local_Env *below;
}Local_Env;

struct Lim_Global_Variables{
	struct{
		FILE *source;
		FILE *destine;
		char *source_name;
		char *destine_name;
		FILE *header_lim;
	}files;
	
	struct{
		bool verbose;
		bool replace;
		bool header_file;
		char *lib_name;
	}flags;

	struct{
		FILE *top_header;
		FILE *code_scope;
		Queue *funct_list;
		Queue *table_list;
	}header_partitions;

	struct{
		FILE *destine_file;
		FILE *scope_fpointer, *scope_faddress;

		Queue *lib_func, *var, *func, *special;

		Queue *func_from_lua, *table_from_lua;
		Queue *func_from_header, *table_from_header;

		unsigned short lenv_quant;
		Local_Env *lenv_stack_top;
	}env_buf;
};

extern struct Lim_Global_Variables lim;
```

> [!NOTE]
> Inicialmente, foi planejado que tal estrutura fosse utilizada por todo o programa,
> entretanto, por conta da arquitetura empregada em `./src/`, essa ideia foi
> descartada. Mesmo assim, o "nome original" da estrutura, e de seus arquivos de origem,
> foi mantidos.

#### O tipo *Queue*

Quando o assunto é *buffer*, o conteúdo detido em `./src/core/tools/queue.c/h` é de
extrema importância. Tais arquivos são responsável por conter o algoritmo encarregado
de inicializar, tratar e liberar **filas**.

> [!IMPORTANT]
> Para mais informações sobre o tipo *Queue*, consulte o tópico: [Filas](#filas).

Essa poderosa estrutura de dados é amplamente utilizada pelos algoritmos de
`./src/core/`, por conta de sua capacidade de armazenar, ordenar e retornar dados.

#### Arquivo opcional

Outro par de arquivos com grande importar é o `./src/core/tools/read-header-lim.c/h`, que,
como seus nomes sugerem, são responsável por conter o algoritmo encarregado de efetuar a
leitura do *header.lim*, o que inclui a busca pelo arquivo, separação de seu conteúdo e
afins.

> [!IMPORTANT]
> Para informações relacionadas ao funcionamento interno desse algoritmo, consulte:
> [Lendo o *header.lim*](#lendo-o-headerlim).

Ao contrário dos demais arquivos presente em `./src/core/`, o conteúdo deste não é
utilizado por funções contidas nesse subdiretório. Sua **única** chamada ocorre na função
`main`, localizada em `./src/main.c`.

#### Tratamento seguro para cadeias

O *Padrão C* conta com inúmeras bibliotecas padrão, destinadas a usos gerais, dentre elas
podemos destacar a `string.h`, a qual fornece funções destinadas ao tratamento de vetores,
com ênfase para cadeias de caracteres.

Por mais que essa seja uma ótima biblioteca, o fato de algumas de suas funções não
possuírem um tratamento adequado para `NULL` às torna discutivelmente inseguras em
múltiplos contextos, como ocorre em alguns processos do Lim.

Pensando nisso, foi desenvolvido um pequeno conjunto de funções, situadas em
`./src/core/string-plus.c/h`, que visam dar o devido tratamento para `NULL` sempre
que o valor for recebido. Estas encontram-se listadas abaixo:

* `char* string_copy(char *str)`: copia o conteúdo de seu argumento para um vetor alocado
dinamicamente e o retorna. Caso `str == NULL`, retornará `NULL`.

* `bool string_compare(char *str0, char *str1)`: compara duas cadeias de caracteres e
retorna o resultado da comparação em *booleano*. Caso uma das cadeias seja igual a `NULL`,
retornará `false`; caso ambas as cadeias sejam iguais a `NULL`, retornará `true`.

* `short string_length(char *str)`: calcula o comprimento de uma cadeia. Caso
`str == NULL`, retornará `-1`.

Entretanto, o conteúdo de `./src/core/string-plus.c/h` não resume-se a "um fragmento
minúsculo das funções de `string.h` contendo tratamento para `NULL`", em seu interior
é possível encontrar duas funções de grandessíssima importância, sendo elas:

1. `void string_set(char **str, STR mode)`
2. `void string_add(char **str, char c)`

Elas fornecem um meio de criar **cadeias de caracteres dinâmicas**, um recurso amplamente
utilizado pelos algoritmos de `./src/core/`, onde:

1. `string_set`: inicializa, reinicia e/ou finaliza uma cadeia de caracteres dinâmica.
2. `string_add`: adiciona uma caractere no final de uma cadeia de caracteres dinâmica.

> [!WARNING]
> Todas as cadeias de caracteres dinâmicas inicializadas **devem** ser finalizadas em
> algum momento.

> [!IMPORTANT]
> `STR` é um tipo baseado no `enum`:
>
> ``` c
> typedef enum{
> 	STR_END,
> 	STR_START,
> 	STR_RESTART,
> }STR;
> ```
>
> Onde suas constante referem-se à ação que deve ser efetuada pela função `string_set`.

#### Modo verboso

Assim como muitos outros programas de terminal, Lim também possui seu próprio
*modo verboso*, o qual pode ser ativado através da utilização da bandeira (*`-V`*)
`--verbose`.

> [!TIP]
> Para obter informações sobre esta e outras bandeiras, consulte:
> [Bandeiras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md)

Essa funcionalidade possui seu próprio algoritmo, que está localizado em
`./src/core/tools/verbose.c/h`, podendo ser acessado por meio da função
`void pverbose(VERBOSE_TAG tag, ...)`.

> [!NOTE]
> `VERBOSE_TAG` é um tipo baseado no seguinte `enum`:
>
> ``` c
> typedef enum{
> 	V_FLAGS_STATUS,
> 	V_HEADER_STATUS,
> 	V_START_PROCESS,
> 	V_CLOSE_PROCESS,
> 	V_CONST_FOUND,
> 	V_IDENT_FOUND,
> 	V_INSERTING,
>	V_WARNING,
> 	V_NEW_THING,
> 	V_LOST_THING,
> 	V_END_THING,
> }VERBOSE_TAG;
> ```
>
> Cujas macros são utilizadas para especificar a `pverbose` qual "tipo" de mensagem deve
> ser exibida.

As mensagem geradas durante o *modo verboso* são referentes à algumas das principais
tarefas do Lim, como um todo.

* Mostrar o estado de definição das bandeiras do Lim.
* Mostrar o estado de saída do *header.lim* e de suas partições.
* Indicar que um dado processo foi iniciado.
* Indicar que um dado processo foi encerrado.
* Exibir as constantes encontradas durante a leitura do arquivo de entrada.
* Exibir os identificadores encontrados, junto a sua "chave de tabela"
(*quando disponível*), durante a leitura do arquivo de entrada.
* Imprime avisos, como forma de justificar certas ações ou a ausência de *algo*.
* Explícita que "algo" (*geralmente um "ambiente"*) foi iniciado/criado, foi
perdido/descartado ou foi encerrado/liberado.

Como forma de facilitar a leitura das mensagens geradas durante o *modo verboso*, ambas
foram separadas em "**tipos**", que são pequenos prefixos à mensagem em questão.

* `[lim]`: texto comum, geralmente exige estados de saída.
* `>lim>`: início de um processo de tamanho considerável.
* `<lim<`: encerramento de um processo de tamanho considerável.
* `|lim|`: uma constante (*ou identificador*) foi encontrada.
* `{lim}`: um dado está sendo inserido em uma dada localizada.
* `[LIM]`: aviso/alerta/justificativa de considerável importância.
* `(lim)`: "algo" está sendo iniciado, foi perdido ou está sendo encerrado.

### O algoritmo principal

Agora que obtivemos conhecimento sobre os mini algoritmos que dão suporte à lógica
principal existente no núcleo, chegou o momento de entendermos o funcionamento do processo
de compactação deste programa.

De modo a facilitar o entendimento desse tópico de discutível complexidade e como forma de
dar a devida profundidade a certos recursos, dividirei essa explicação, visando introduzir
cada um dos arquivos que compõem o algoritmo principal do núcleo antes de explicar seu
funcionamento como um todo.

#### O cérebro

Assim como a lógica de `./src/args/`, o algoritmo principal do núcleo também possui um
"arquivo mestre", denominado como `./src/core/process-master.c/h`, o qual é responsável
por efetuar a:

* Leitura parcial do arquivo de entrada: seu conteúdo é capturado, em trechos, e enviado
para ser devidamente tratado.

* Construção do arquivo de saída: utiliza, em uma dada ordem, todo o conteúdo extraído do
arquivo fonte para construir um arquivo de saída.

#### Verificando trechos

Por mais que `./src/core/process-master.c` detenha a função responsável por ler o arquivo
de entrada, é o conteúdo de `./src/core/check-content.c/h` que faz o trabalho pesado.

Tal arquivo é formado por uma série de funções, especializadas em identificar trechos
capturados e destiná-los ao seu devido tratamento. Ambas encontram-se abaixo:

* `bool clear_white_spaces(char *c)`: "pula" caracteres de espaçamento.

* `bool is_number(char c, char **tmp)`: captura conjuntos de caracteres iniciados com
dígitos.

* `bool is_string(char c, char **tmp)`: captura cadeias de caracteres iniciadas com aspas
(*simples ou duplas*). Respeita barras invertidas.

* `bool is_table(char c, char **tmp)`: captura ambientes de tabela. Remove espaçamentos
desnecessários e comentários. Respeita cadeias de caracteres; respeita a abertura de novos
ambientes de tabela.

* `bool is_commentary(char c)`: identifica e descarta comentários, seja em linha, seja em
bloco.

* `void is_special_char(char c)`: captura conjuntos de caracteres formados **apenas** por
caracteres especiais. Remove espaçamentos e comentários. Segue algumas regras:
	1. Conjuntos iniciadas com `','` não devem ter comprimento maior que um.
	2. O segundo caractere de conjuntos iniciados com `'='` deve ser igual a `'='`.
	3. `_'{"` são inválidos para esse operação e desencadeiam seu fim.

* `bool is_identifier(char c, char **tmp)`: captura conjuntos iniciados com alfabéticos ou
`'_'`, identifica seu tipo e envia o trecho para o devido tratamento. As cadeias coletadas
são divididas em:
	* Palavra-chave de *Lua*.
	* Função ou tabela (*biblioteca*) do *Padrão Lua*.
	* Identificadores registrados no *header.lim*.
	* Identificadores declarados no arquivo de entrada.

> [!IMPORTANT]
> Todas essas funções, exceto `clear_write_spaces` e `is_special_char`, retornarão `false`
> caso seu primeiro teste falhe; `clear_write_spaces` retornará `false` se o final do
> arquivo de entrada não for encontrado.

> [!NOTE]
> Funções que possuem o parâmetro `char **tmp` o utilizarão para armazenar todo o conteúdo
> capturado, com exceção de `is_identifier`, que fará isso apenas se tal trecho for uma
> palavra-chave de *Lua*, do contrário o fragmento será enviado diretamente para
> tratamento. Aquelas que não o possuem descartarão os conteúdos coletados.

> [!TIP]
> Essa tabela indica quais encontros, ocorridos após `c` ser "aprovado", desencadeiam o fim
> das operações de cada função acima:
>
> | Funções            | Alfab. | Dígito | C. Especial | Espaçam. | Específ. |
> | :--                | :-:    | :-:    | :-:         | :-:      | :-:      |
> | clear_white_spaces | *      | *      | *           |          |          |
> | is_number          |        |        | *           | *        |          |
> | is_string          |        |        |             |          | `'`, `"` |
> | is_table           |        |        |             |          | `}`      |
> | is_commentary      |        |        |             |          | `\n` `]]`|
> | is_special_char    | *      | *      |             |          | `_'{"`   |
> | is_identifier      |        |        | *           | *        |          |
>
> * *Alfab* também engloba `'_'`.
> * *C. Especial* não inclui `'_'`.
> * Todos são finalizados após um encontro com o final do arquivo.

#### Tratando fragmentos

Agora que vimos de que modo os dados são coletados, adentraremos
`./src/core/check-content.c/h` e descobriremos como eles são tratados.

O conjunto de funções presente em `./src/core/check-content.c/h` permite que certas
cadeias possam modificar o modo como cadeias futuras serão tratadas, por exemplo:

> Caso a palavra chave `local` seja recebida, o algoritmo entenderá que uma declaração de
> variável(*is*) ou função está preste a ocorrer, de modo a que se:
>
> * O próximo conjunto for um identificador, a declaração de variável será confirmada.
> * O próximo conjunto for igual à `function`, a declaração da função será confirmada.
> * Do contrário, o **contexto** será redefinido para nulo.

Seu algoritmo é formado por três funções principais, sendo elas:

* `void treat_const(char *str)`: recebe apenas conjuntos que não deverão ser compactados,
e, com base neles, define o contexto atual.

* `void treat_ident(char *_ident, char *_table_key)`: recebe, majoritariamente, cadeias que
deverão ser compactados, as quais poderão ser utilizados para definir/atualizar o que é
esperado para o contexto atual.

* `void treat_standard(char *_ident, char *_table_key, Queue **buf)`: recebe
identificadores (*tabelas e funções*) do *Padrão Lua* e do *header.lim* para compactação.
Redefine o contexto atual para nulo, independente de qual seja.

> [!IMPORTANT]
> Ambas as funções listadas acima são encarregadas de atualizar os ponteiros estáticos
> `char *gident` e `char *gtable_key`, os quais são utilizados por diversas funções para
> as mais variadas operações.
>
> * `treat_const` define: `gident = str; gtable_key = NULL;`.

> [!TIP]
> Existem cinco contextos:
> 
> * Nulo.
> * Inicialização de declaração.
> * Declaração de variável(*is*) local(*is*).
> * Declaração de parâmetro(*s*).
> * Declaração de identificador(*es*) de blocos *for loop*.
> 
> Os contextos são utilizados para definir quando é necessário salvar novos apelidos e
> quando é necessário solicitar apelidos já definidos.

As demais funções desses arquivos encontram-se listadas abaixo:

* `default_const_treatment(char *str)`: executa um tratamento padrão para a cadeia dada a
`str`.

* `declare_function(bool is_anony)`: define a estrutura de declaração de uma dada função e
define o contexto atual para: declaração de argumentos.

* `search_fparam_end(void)`: utilizada para verificar o fim do contexto de "declaração de
argumentos".

* `check_mandatory_space(char *str)`: verifica se é um espaço obrigatório é necessário e o
adiciona ao *buffer*, caso a condição prove-se verdadeira. Além de atualizar um dado
estado, para futuras verificações.

> [!NOTE]
> Todas elas são estáticas, além de não possuírem nenhum retorno.

#### Escopos de ambientes

Assim como a maioria das linguagens de programação, *Lua* também possui dois escopos de
identificadores:

* Global: disponível em todo o arquivo, a partir de sua declaração.
* Local: disponível apenas em um dado bloco, a partir de sua declaração.

Dado que esse recurso possibilita a reutilização de nomes de identificadores, foi
desenvolvido um sistema de ambientes, capaz de reutilizar apelidos e descartar
(*da memória*) informações não mais necessárias, assim permitindo uma código de saída mais
compacto e um processo de compressão mais otimizado.

Para gerenciar esses ambientes, um conjunto de funções foi desenvolvido e depositado em
`./src/core/layer-env.c/h`, as quais estão listadas abaixo:

* `static bool is_valid_open(char *kw, char *tk)`: verifica se a palavra-chave (*`kw`*)
recebida indica a declaração de algum bloco de código suportado. `tk` é utilizado para
verificar a declaração de métodos e funções.

* `static bool is_valid_clse(char *kw)`: verifica se há algum ambiente na pilha e se a
palavra-chave recebida é capaz de encerra o ambiente atual.

* `void add_layer_env(char *kw, char *tk)`: cria uma nova camada para a pilha de ambientes
e salva o estado atual de *alguns* apelidos em seus membros.

* `void pop_layer_env(char *kw, bool force)`: remove a última camada da pilha de ambientes
e atribuí os estados salvos aos seus determinados "*buffers* de apelidos".

* `void finish_forgot_env(void)`: encerra todos os ambientes na pilha, forçadamente.

> [!TIP]
> O ponteiro encarregado de armazenar essa pilha localiza-se no membro
> `lim.env_buf.lenv_stack_top`.

Abaixo encontra-se a estrutura utilizada para construir as camadas da pilha de ambiente:

``` c
typedef struct Local_Env{
	FILE *content;
	
	char *save_local_ident;
	char *save_parameter;
	char *save_for_loop;
	
	Queue *var, *func, *special;
	
	Local_Env_Type type;
	struct Local_Env *below;
}Local_Env;
```

* `content`: armazena o conteúdo, oriundo do arquivo de entrada, capturado enquanto o
ambiente em questão estava "ativo".

* `save_*`: detém o estado de dadas "classes" (*local, parâmetro e "for loop"*) de
apelidos, que será utiliza para reaproveitar os apelidos utilizados no novo ambiente.

* *Filas*: armazenam os identificadores e apelidos locais encontrado no bloco/ambiente
atual.

* `type`: tipo do ambiente, utilizado para realizar operações específicas.

* `below`: próximo item a ocupar o topo da pilha.

> [!IMPORTANT]
> `content` existe para possibilitar a implantação de trechos em seu conteúdo, a qual
> ocorre durante o envio de seus dados para um determinado destino
> (*após seu encerramento*).
> 
> A exemplo, é possível citar a inclusão de `local Sa=self;`, que ocorre em métodos, após
> o fim da declaração de seus parâmetros.

> [!NOTE]
> `special` refere-se a parâmetros de funções ou identificadores declarados junto a blocos
> *for loop*.

> [!TIP]
> `Local_Env_Type` é um tipo utilizado para definir a tipagem de diferentes ambientes,
> assim definindo tratamentos específicos para o mesmo. Este baseia-se no seguinte `enum`:
> 
> ``` c
> typedef enum{
> 	LET_NULL,
> 	LET_FUNC,
> 	LET_METHOD,
> 	LET_REPEAT,
> 	LET_OTHER,
> }Local_Env_Type;
> ```
> 
> O qual situa-se em `./src/core/tools/lim-global-variables.h`.

#### Apelidos

Como último par de arquivos temos `./src/core/nick-man.c/h`, os quais abrigam uma gama de
funções, vetores e afins destinados à manipulação de apelidos.

No contexto desse programa, apelidos referem-se ao "identificador curto" utilizado para
substituir os identificadores embutidos no arquivo de entrada.

Os apelidos são formados por cadeias de caracteres alfabéticos, os quais podem ser
minúsculos ou maiúsculos, prefixados ou não, a depender de sua "classe".

A lógica de seu algoritmo é simples, sendo constituída por operações matemáticas envolvendo
valores do tipo `char`, a qual assemelha-se bastante com a abordagem utilizadas pelos
numerais decimais. Observe esse exemplo, onde as linha representam uma atualização do valor
presente acima:

```
a  | fh | zzz  | mkz
aa | fi | aaaa | mla
ab | fj | aaab | mlb
```

Esse poderoso algoritmo é composto, principalmente, pelas seguintes funções:

* `void start_nickname_buffers(void)`: inicializa os *buffers* responsáveis por armazenar o
estado atual de cada "classe" de apelido.

* `void save_nicknames_state(Local_Env *lenv)`: utilizada (*por `add_layer_env`*) para
salvar o estado atual dos apelidos das "classes": local, parâmetro e *for loop*.

* `void restart_nicknames_state(Local_Env *lenv)`: utilizada (*por `pop_layer_env`*) para
redefinir o estado de dados apelidos (*das "classes": local, parâmetro e "for loop"*) para
o último valor salvo.

* `char* save_ident(char *ident, char *table_key, NICK_ID id, Queue **buf)`: salva um
identificador na memória (*`*buf`*) e lhe atribuí um apelido, com base em `id`.

* `char* get_nickname_of(char *ident, bool is_std_hdr)`: busca por um apelido para `ident`,
na seguinte ordem:
	* Se `is_std_hdr == true`:
		1. Variáveis locais.
		2. Funções locais.
		3. Parâmetros/identificador de blocos *for loop* (*locais*).
		4. Variáveis "privadas".
		5. Funções "privadas".
		6. Funções adicionadas à biblioteca.
	* Do contrário:
		1. Funções do *Padrão Lua*.
		2. Tabelas do *Padrão Lua*.
		3. Funções registrados no *header.lim*.
		4. Tabelas registrados no *header.lim*.

* `void free_nickname_buffers(void)`: encerra os *buffers* responsáveis por armazenar o
estado atual de cada "classe" de apelido.

> [!IMPORTANT]
> Os *buffers* citados acima são vetores do tipo `char*`, que armazenam:
>
> * O estado atual do apelido.
> * O apelido atual (*prefixo + estado atual*).
> * O primeiro caractere da sequência de incremento (*"constante"*).
> * O último caractere da sequência de incremento (*"constante"*).
> * O caractere utilizado como prefixo para os apelidos (*"constante"*).
>
> Existem cinco *buffers* de apelidos, os quais pertencem às seguintes "classes":
>
> * Padrão e registrados: apelidos dados a identificadores oriundos do *Padrão Lua* e do
> *header.lim*.
> 	* Formados por letras maiúsculas.
>
> * Globais: apelidos para variáveis, tabelas e funções declaradas dentro do "ambiente
> raiz", por meio da palavra-chave `local`, ou seja, para identificadores "privados".
> 	* Prefixados por `G`.
>
> * Locais: apelidos para variáveis, tabelas e funções declaradas dentro de blocos/
> ambientes, por meio da palavra-chave `local`.
> 	* Prefixados por `L`.
>
> * Parâmetro: apelidos para parâmetros de funções.
>
> * *For loop*: apelidos para identificadores declarados durante a criação de blocos
> *for loop*.
> 	* Prefixados por `F`.

> [!NOTE]
> `save_ident` não irá registrar identificadores iniciados com `'_'`, estes serão
> retornados como estão, com exceção de `"_"`, que desencadeará o retorno de `"__"`. O
> mesmo vale para `get_nickname_of`, em questão de retorno.
> 
> Se `get_nickname_of` não encontrar um apelido para o identificador dado como argumento,
> ela retornará o próprio argumento.

Ao contrário dos identificadores declarados dentro do arquivo de origem, aqueles oriundos
do *Padrão Lua* e do *header.lim* **não** podem ser compactados durante sua declaração,
por conta disso, no topo do contêiner da biblioteca, é declarado um "escopo de
referências", que consiste em uma estrutura de declarações cujos identificadores recebem
uma dada função ou tabela originada no *Padrão Lua* ou no *header.lim*. Observe o exemplo
abaixo:

``` lua
local A,B,C,D,E=print,math.random,type,string,string.sub;
```

Diferente das chaves de tabela oriundas do arquivo de entrada (*que não são compactadas*),
estas são fundidas com sua tabela de origem e são tratadas como um único identificador. Com
relação ao processo de adição desses identificadores a tal escopo, este ocorre por meio de
chamada à função `save_ident`.

> [!NOTE]
> `NICK_ID` é um tipo utilizado para definir que "classe" de apelido será utilizada para
> apelidar um dado identificador. Tal tipo tem base no seguinte `enum`:
> 
> ``` c
> typedef enum{
> 	NICK_STD_HDR,
> 	NICK_IDENT,
> 	NICK_PARAM,
> 	NICK_FOR_LOOP,
> 	NICK_LIB_FUNC,
> }NICK_ID;
> ```

#### Linearmente falando

Visto que todos os arquivos que compõem o "algoritmo principal" do núcleo foram
esclarecidos, chegou a hora de retomarmos a explicação linear que ocorria anteriormente.

Bom, agora que voltamos à `main`, e ao fluxo de execução do programa, estamos frente a
frente com uma estrutura de código familiar:

``` c
lim_init_env();
atexit(lim_free_env);

read_source_file();
```

Esta é responsável por inicializar a estrutura global de `./src/core/` e por chamar a
função que detém o algoritmo de leitura e compactação, o qual extrairá, compactará e
armazenará o conteúdo existente no arquivo de entrada.

##### Compactando o arquivo de entrada

Ao adentrarmos `read_source_file` podemos ver a *inicializações* de uma série de
*utilitários*, os quais serão necessários para os processos posteriores. Sendo eles:

``` c
char *tmp = NULL;

lim.files.source = fopen(lim.files.source_name, "r");
lim.env_buf.destine_file = tmpfile();

atexit(finish_forgot_env);
start_nickname_buffers();
```

Seguidamente, uma repetição é iniciada, a qual é responsável por efetuar a leitura do
arquivo de entrada. Para isso, tal *loop* extraí um caractere desse arquivo e o submete a
uma série de testes e validações, os quais são desempenhados por funções oriundas de
`./src/core/check-content.c/h`.

Sempre que uma *constante* for encontrada, a mesma será armazenada em um endereço dinâmico,
o qual será utilizado como argumento para `treat_const`
(*em `read_source_file`, no ciclo posterior*). Tal função realizará uma bateria de
verificações, até que o valor recebido seja impresso no *buffer* de destino.

> [!NOTE]
> `treat_const` é responsável por chamar `add_layer_env` e `pop_layer_env`; além disso,
> essa função é a única capaz de inicializar contextos, o que é feito antes do conteúdo
> em questão ser gravado na memória.

Já os identificadores, como dito anteriormente, não são dados a `tmp`, ao invés disso
`is_identifier` irá destiná-los, diretamente, ao seu devido tratamento em uma das
seguintes funções:

* `treat_ident`: receberá identificadores declarados no arquivo de entrada.
* `treat_standard`: receberá identificadores oriundos do *Padrão Lua* e do *header.lim*.

> [!NOTE]
> Para conseguir distinguir a origem de cada cadeia extraída, `is_identifier` compara o
> valor obtido com aqueles originados no *Padrão Lua* e no *header.lim*, caso um valor
> igual seja encontrado em seus *"buffers"*, `treat_standard` será chamada, caso contrário
> `treat_ident` será chamada.

Depois da leitura do arquivo fonte ser concluída, as funções de encerramento a seguir serão
chamadas:

``` c
finish_forgot_env();
free_nickname_buffers();
string_set(&tmp, STR_END);
```

Marcando assim o fim do algoritmo de compactação.

##### Construindo o arquivo de saída

Com o fim de `read_source_file`, estamos de volta à `main`, dessa vez para realizar a
chamada da última função presente em tal estrutura de código, sendo ela
`void build_destine_file(void)`.

Como seu nome dá a entender, esta é responsável por efetuar a construção do arquivo de
destino, ou seja, ela vai criar um arquivo persistente e, em uma dada ordem, imprimir nele
*todo* o conteúdo extraído do arquivo de entrada.

Seu algoritmo é bem simples, consistindo apenas na transferência dos dados
(*e na impressão de mensagens verbosas*). Tal compartilhamento é efetuada na seguinte
ordem:

```
-- FOO +-> Partição: top header
-- FOO |   [do "header.lim"]
-- FOO *
```

> Assim como *code scope*, esse trecho é opcional, pois depende da utilização do
> *header.lim*. Nele é possível imprimir qualquer tipo de conteúdo. Veja também:
> [O *header.lim*](https://github.com/duckafire/lim/blob/main/docs/o-header-lim.md).

```
      +-> Declaração da "tabela núcleo"
      |
<-------->
local _={}
```

> A *tabela núcleo" é responsável por armazenar todo o conteúdo "público" presente na
> biblioteca. Seu identificador foi pensado para ser mínimo e para não gerar conflito com
> os apelidos gerados pelo Lim.
> 
> Caso um identificador nomeado como `"_"` seja encontrado no arquivo de entrada, ele será
> *apelidado* como `"__"`.

```
 +-> Contêiner da biblioteca <-----------------------------------------+
 |                                                                     |
 |                 +> Partição: code scope                             |
 |                 |  [do "header.lim"]                                |
<>               <----------------------->                            <->
do local P=print;local _txt="Hello world!";function _.msg()A(_txt)end end
   <------------>                          <------------------------>
    |                                        |
    +-> Escopo de identif.                   +-> Conteúdo da
        do "Padrão Lua" e                        biblioteca
        do "header.lim"
```

> Essa é a estrutura base da biblioteca, a qual expande-se ilimitadamente para a direita.
> 
> Os identificadores declarados no "ambiente raiz" são classificados como:
> 
> * "Público": podem ser acessadas pelo código contido em qualquer parte do projeto que
> seja posterior a declaração da biblioteca, por meio da "tabela de referência".
> 
> * "Privado": só podem ser acessados pelo código está presente dentro da biblioteca.

```
local lib=_
<--------->
       |
       +-> "Tabela de referência"
           à biblioteca
```

> Dado que, todas as "tabelas núcleo" possuem o mesmo identificador, as "tabelas de
> referência" foram pensadas para deter a custodia do conteúdo "publico" da biblioteca,
> assim evitando a perda dele após a declaração de outra biblioteca.

``` lua
-- FOO
-- FOO
-- FOO
local _={}
do local P=print;local _txt="Hello world!";function _.msg()A(_txt)end end
local lib=_
```

> Baseado nos trechos acima, esse seria o código de saída.

Ao fim desse rápido processo de transferência de dados, o fim de `build_destine_file` é
alcançado, assim levando o fluxo de volta à `main`, que encerra o programa retornando `0`.

---

# Considerações finais

***Uau!*** Escrever isso foi uma experiência discutivelmente agradável, um desafio complexo
e difícil, não porque eu não entendia o código, mas sim porque era custoso por meu
conhecimento em palavras.

Quando iniciei esse artigo, eu sabia que meu trabalho nele não seria breve, mas não poderia
imaginar que essa empreitada duraria mais de **duas semanas e meia**. Certamente boa parte
desse tempo foi gasta com correções de *bugs* e erros, além de melhorias no código
(*sem mencionar minha vida pessoal*).

Essa pequena aventura me proporcionou muito aprendizado, principalmente com relação a
escrita de documentações e ao uso do *Markdown*.

Foi um pouco estranho "criar" termos para nomear certas características do Lim e de seu
código, mas foi divertido. Todavia espero que isso não tenha tornado esse assunto
desnecessariamente complexo.

Uma das melhores decisões que eu tomei, antes de iniciar essa pequena documentação, foi de
não escrevê-la em inglês, isso me poupou **MUITO** tempo e me permitiu ser mais expressivo.
*Como sei disso?* Bem, digamos que eu já tenha me desafiado a fazer algo do tipo, em escala
menor, de maneira mais simples, mas fiz.

Inicialmente eu pretendia explicar cada *milimetro* do código, mas isso séria
exageradamente desnecessário, visto que o código deste projeto foi estruturado para ser o
mais legível possível (*espero que tenha dado certo*).

Acho que isso é tudo. Por fim, eu gostaria de agradecê-lo por ler *isto*, espero que tenha
sido uma leitura agradável e clara, capaz de lhe sanar toda e qualquer dúvida relacionada a
este projeto. Até qualquer hora.

*- DuckAfire*

---
