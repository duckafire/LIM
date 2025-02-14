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
				* [Arquivo de destino](#arquivos-de-destino)
				* [Tabela de referência](#tabela-de-referência)
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
		* [Aprontando as saída](#aprontando-as-saídas)


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
facilitar a compilação do programa, quanto para auxiliar desenvolvedores que queiram
manipulá-lo.

Todo o código fonte do Lim está retido dentro do subdiretório `./src/`, logo, ao falarmos
da *arquitetura do projeto*, é da organização desse subdiretório que estamos falando.
Entretanto, antes de nos aprofundarmos nele, é importante conhecermos um pouco sobre os
demais subdiretórios, afinal, como dito anteriormente, seu conteúdo existe para facilitar
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

O subdiretório `./tests/` é destinado a armazenar uma série de algoritmos
(*escritos em `Bash Script`, `C` e `Lua`*), encarregados de explorar essa característica
para fins de depuração.

Antes de prosseguir, vamos vislumbrar o conteúdo desse subdiretório:

```
run
scripts/
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

* `args`: lista de argumentos que serão utilizados nos testes. Geralmente é uma lista de
índices numéricos, de `0` a `n`.

* `files`: arquivos fonte do Lim que são necessários para a compilação do executável
de depuração. Geralmente envolve um arquivo principal e "pequenas" dependências.

* `main.c`: detém uma lógica específica para alimentar, manipular e exibir as saídas dos
algoritmos.

> [!NOTE]
> `debug-common.h` contém algumas macros "facilitadoras", utilizadas em `main.c`.

Agora que o intuito desse padrão foi compreendido, chegou o momento de entender como
o algoritmo responsável por manipulá-los funciona.

Esse é formado pelos *scripts*: `run`, `create-main` e `start-tests`. Abaixo encontra-se o
propósito de cada um:

* `run`: captura os argumentos que lhe foram passados e, baseado neles, define quais
ações serão tomadas. Define variáveis, constantes e funções para uso posterior nos 
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
> (*gerados por `run` em `./tests/script/.tmp/*`*), durante a execução de `start-tests`
> *poderá* causar erros.

`run` possui seus próprios argumentos, os quais foram pensados para facilitar ainda mais o
processo de análise. Ambos encontram-se abaixo:

* *Nenhum*: exibe uma mensagem similar a esta: `./run < keyword | flag > [ flag ]`

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
desenvolvido com o intuito cobrir os seguintes pontos:

* Limitar o "alcance" de interação entre arquivos, de modo a evitar que arquivos "olhem
para trás".

* Separar trabalhos distintos em arquivos diferentes, nomeados de maneira a explicitar sua
tarefa.

* Organizar claramente a ordem em que os processo ocorrem.

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

Como forma de evitar o surgimento de uma grande teia de aranha, o conceito de "isolamento" foi empregado. Este consiste em limitar a interação entre arquivos, de modo a que eles
possam comunicar-se apenas com outros que encontram-se na mesma pasta ou dentro de
subpastas também localizada na mesma pasta. Ou seja, um dado arquivo não pode comunicar-se 
com outros que estejam posicionados anteriormente. Veja o exemplo:

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
> pastas afrente, como `A/B/C/D/E/a.file`, isso não é recomendado. Limite-se a no máximo
> duas ou três pastas.

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

* `./src/main.c`: responsável por orquestrar todo o funcionamento do programa, através da
inicialização de estruturas globais, chamada de "funções centrais" e afins.

* `./src/args/process-master.c`: responsável pela verificação dos argumento passados ao
Lim, por meio do uso do conteúdo de `./src/args/*`. Sua "função central" chama-se
`check_program_arguments`.

* `./src/core/process-master.c`: responsável por efetuar a compactação dos arquivos
passados ao Lim como argumento, por meio do uso do conteúdo de `./src/core/*`. Sua "função
central" chama-se `read_source_file`.

---

# Por trás dos panos

Chegamos na parte mais entusiasmante da documentação, o momento em que o funcionamento do
código deste projeto será explicado, no caso, o conteúdo contido em `./src/`. Primeiro,
vamos dar uma olhada nesse diretório:

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
         +-> ident-man.c/h
         +-> process-master.c/h
         +-> treat.c/h
         +-> tools/
               +-> lim-global-variables.c/h
               +-> queue.c/h
               +-> read-header-lim.c/h
               +-> string-plus.c/h
               +-> verbose.c/h
```

Agora que você adquiriu conhecimento sobre **todo** o conteúdo de `./src/`, iremos navegar
dentro de seus subdiretórios, em busca de entender o funcionamento de todo o seu interior.

## Cabeçalhos do *Cmake*

Vamos começar em ordem crescente: do mais fácil ao mais difícil. Baseado nessa ordem,
falaremos agora sobre o subdiretório `./src/cmake-h`.

Como é sabido por todos aqui (*creio eu*), o *Cmake* possibilita a criação de "arquivos
de entrada", que podem ser usados para gerar outros arquivos, os quais poderão possuir
informações contidas no `CMakeLists.txt`, como valores de variáveis.

Sabendo disso, este diretório foi criado para armazenar, tanto os "arquivos de entrada"
(*criados por nós, desenvolvedores*), quanto os "arquivos de saída"
(*criados pelo Cmake e ignorados pelo Git*).

> [!NOTE]
> Use a extensão `.h.in` em "arquivos de entrada".

## O grande maestro

Assim como todo projeto escrito em *C*, este também possui uma função `main`, localizada
em `./src/main.c` (*sendo a única presente no arquivo*), que é um "arquivo mestre". Seu
código é mínimo, mas seu trabalho é de extrema importância, consistindo em:

1. Capturar os argumentos do programa.
2. Inicializar os membros de estruturas globais.
3. Adicionar funções de "liberação" à pilha da função `atexit`.
4. Fazer a *comunicação* entre diferentes algoritmos.
5. Chamar "funções centrais", as quais, majoritariamente, originam-se de outros mestres.

## Verificando e validando argumentos

Chegou o momento de tratar de um algoritmo mais complexo. Falaremos agora sobre a lógica
responsável por verificar e validar os argumentos passados ao Lim, a qual localiza-se em
`./src/args/`.

É de grande importância entendermos todo o contexto desse conteúdo antes de realmente
partirmos para a explicação desse algoritmo, por conta disso, iremos navegar por este
subdiretório, visando compreendê-lo.

### Mensagens fatais

Começaremos por uma pasta de nome peculiar: `fatal/`. Seu conteúdo consiste em arquivos
de cabeçalho, formados por macros padronizadas, onde ambas:

* Imprimem uma mensagem diferente.
* Encerram o programa.

Elas estão divididas em dois grupos:

* `error.h`: encerra o programa após um dado erro durante a verificação dos argumentos. Cada macro possui seu próprio código de erro. Veja:
[Códigos de erro](https://github.com/duckafire/lim/blob/main/docs/codigos-de-erro.md).

* `info.h`: encerra o programa após uma *bandeira informativa* ser encontrada. Todas as
macros encerram o programa com sucesso (`0`). Veja:
[Bandeiaras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md).

A estruturas de tais macros consiste na seguinte:

``` c
#define ERROR(code, str) \
	puts(str "\n"); \
	exit(code)

#define ERROR_foo \
	ERROR(1, "foo")
```

### Bandeiras

Chegou o momento de adentrarmos em `flags/` e, como é de se imaginar, seu conteúdo é
voltado às bandeiras do Lim.

> [!IMPORTANT]
> Para informações especificas sobres as bandeiras (*como: usos, tipos e afins*), consulte:
> [Bandeiras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md)

Seu interior é compostos por alguns poucos arquivos, responsáveis pela:

* `flags/list.h`: Declaração das bandeiras, no formato de constante de pré-processamento
(*ambas prefixadas por `FLAG_`*).
* `flags/cmp.c/h`: Declaração da "função de comparação" (`flag_cmp`).

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
> anteriormente, tais constantes não são utilizadas para tal, logo não há problema algum no
> uso dessa abordagem.

Para realizar as comparações, entre os argumento e as bandeiras, foi desenvolvida a
seguinte função:

``` c
bool flag_cmp(char *arg, char *f, char *flag){
	return (strcmp(arg, f) == 0 || strcmp(arg, flag) == 0);
}
```

> [!NOTE]
> Veja esse exemplo de uso:
> ``` c
> if( flag_cmp("foo arg", FLAG_FOO) );
> 	puts("Hello world!\n");
> ```

Dito isso, podemos concluir que:

* As "representações gráficas" das bandeiras são usadas apenas em comparações (entre elas e
uma dado argumento).

* Ambas as versões de uma mesma bandeira são detidas dentro da mesma constante de
pré-processamento.

* As constantes, que detêm as bandeiras, são utilizadas apenas como argumento para a função
`flag_cmp`.

> [!IMPORTANT]
> Vale ressaltar que, além das bandeiras do Lim, constantes para palavras-chave de
> bandeiras também são definidas em `flags/list.h`, como é o caso de (`l`) `list`,
> palavra-chave da bandeira (`-h`) `--help`. Ambas seguem o mesmo padrão de estrutura.

### Mensagens de auxílio

Partindo agora para o conteúdo raiz de `./src/args/`, falaremos brevemente sobre o que está
contido em `help.c/h`, que, como é de se imaginar, é voltado à ajuda
(*durante a execução do programa*). Essas informações podem ser acessadas através do uso da
bandeira (`-h`) `--help`
(*veja: [Bandeiaras](https://github.com/duckafire/lim/blob/main/docs/bandeiras.md)*).

 As **funções** presentes nesses arquivos seguem o mesmo conceito das macros presentes em
`args/flags/info.h`, sendo eles:

* Exibir uma mensagem.
* Encerrar o programa com sucesso.

> Com o acréscimo de verificarem o conteúdo recebido.

> [!NOTE]
> Por conta da arquitetura desse projeto, `help.c/h` não pode ser posicionado dentro de
> `args/fatal/`.

### O processo em si

Enfim, chegou o momento de entendermos esse algoritmo. Mas primeiro, é necessário
conhecermos sua estrutura global e seus membros:

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

* `index`: último índice do vetor de argumentos (`argv`).
* `**value`: ponteiro para o vetor de argumentos.
* `content_version`: cadeia de caracteres contida em `LIM_VERSION`.
* `content_shared`: indica se os valores armazenados nesta estrutura foram compartilhados
com a estrutura global de `./src/core/`.
* `files_name.*`: nome dos arquivos de origem e destino, ambos atribuídos ao Lim.
* `flags.*`: estado das bandeiras (ativada, desativada) e "conteúdos". 

> [!NOTE]
> Se `args.content_shared` for verdadeiro, a função `args_free_env` terá seu efeito
> "anulado", pois a custodia de seus endereços dinâmicos será dada à estrutura global de
> `./src/core/`.

Os processos desse algoritmo iniciam-se em `./src/main.c`, após a inicialização de sua
estrutura global, com a chamada da função `void check_program_arguments(void)`, veja:

``` c
int main(int argc, char *argv[]){
	args_init_env(argc, argv, LIM_VERSION);
	atexit(args_free_env);

	check_program_arguments();
```

`check_program_arguments`, função declarada em `args/process-master.c/h`, é responsável
por efetuar chamadas para funções presentes em `args/check.c/h` e `args/treat.c/h`, na
seguinte ordem:

#### Buscando bandeiras informáticas

A primeiro coisa que `check_program_arguments` faz é chamar a função de verificação
`void is_it_information_flag(void)`. Tal função é responsável por verificar o primeiro
argumentos passado ao Lim, onde:

1. Se não houver nenhum argumento, uma mensagem de padrão será exibida.
2. Caso uma *bandeia informativa* seja encontrada, sua respectiva mensagem será exibida.
3. Do contrário, o fluxo do programa prosseguirá.

> [!NOTE]
> Caso a segunda condição seja verdadeira e hajam mais argumentos, será verificado se tal
> bandeira possui suporte a palavras-chave, caso sim, sua respectiva função será chamada
> com ele como argumento, do contrário, o mesmo será ignorado e um aviso indicando isso
> será imprimido na saída do programa.
> 
> Se o sufixo for obrigatório, um erro fatal ocorrerá; caso hajam argumentos além daqueles
> requeridos pela bandeira, eles serão ignorados e um aviso indicando isso será imprimido
> na saída do programa.

#### Buscando o arquivo de entrada

Caso o programa não seja interrompido na etapa anterior, será intendido que a intensão do
usuário é compactar um arquivo. Então o fluxo do programa retornará a função
`check_program_arguments`, que chamará a função de verificação
`void search_and_set_source_file(void)`.

O objetivo de `search_and_set_source_file` é verificar se o primeiro argumento passado ao
Lim é o nome de uma arquivo válido.

> [!IMPORTANT]
> Ou seja, caso o primeiro argumento dado ao Lim não seja uma *bandeira informativa*, ele
> deverá ser um nome de arquivo.

Para efetuar esse verificação, o Lim realizará os seguintes "testes":

1. O argumento inicia com `'-'`.
2. O arquivo não existe.
3. O arquivo não possui a extensão `.lua`.

Caso algumas dessas afirmações seja verdadeira, um respectivo erro fatal ocorrerá.

> [!WARNING]
> O nome do arquivo de origem não deve começar com '-'.

Em meio a esses processos, o membro `args.files_name.source` receberá o primeiro argumento
passado ao Lim, que será usado para abrir o arquivo entrada para leitura em processos
posteriores.

#### Verificando os demais argumentos

Agora que o primeiro arquivo foi tido como sendo um nome de arquivo válido, voltaremos
a `check_program_arguments`, dessa vez, para chamar a função de verificação
`void read_other_arguments(void)`.

`read_other_arguments` foi criada para verificar os demais argumentos que foram passados
ao Lim, mas isso, primeiro ela verifica se há mais algum argumento além do primeiro, em
caso negativo, ela se interromperá e retornará a `check_program_arguments`, em caso
positivo, ele fará o seguinte.

1. Iniciará um *loop* para ler os argumentos (*partindo do segundo*).
2. Comparará o argumento "atual" com todas as bandeiras.

Em relação ao "ponto dois", se o argumento for:

* Igual a uma *bandeira informativa*.
* Uma *bandeira de ação* que exija um sufixo, mas que esteja sem.
* Uma bandeira já utilizada na chamada (*repetida*).
* Uma bandeira inválida.
* Ou um argumento inválido (*qualquer valor "estranho"*).

Um erro fatal ocorrerá.

> [!NOTE]
> Sempre que uma bandeira válida for encontrada, seu membro respectivo na estrutura `args`
> terá seu valor alterado.

#### Definindo valores omitidos

Como sabemos, a especificação do nome (*e caminho*) do arquivo de saída e o nome da tabela
de referência (*para a biblioteca*) são argumentos opcionais, entretanto eles são valores
indispensáveis para os processos do Lim, por conta disso, faz-se necessário que tal
programa defina um "valor padrão" para esses campos.

Sabendo disso, após a chamada da função de verificação `read_other_arguments`, em
`check_program_arguments`, é verificado se tais valores foram especificados e corretamente
salvos pelo processo anterior, caso não, algumas funções serão chamadas para executar
um tratamento padrão.

##### Arquivo de destino

Em um cenário no qual o arquivo de saída tenha seu nome e caminho omitidos,
`check_program_arguments` chamará a função de tratamento
`void set_destine_file_name(const char *name)`, usando `args.files_name.source` como
argumento.

Tal função copiará o conteúdo de `args.files_name.source` para um endereço dinâmico,
substituirá sua extensão por `.lim` e o atribuirá tal endereço a `args.files_name.source`.
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

1. O conteúdo de seu argumento é copiado para uma variável local.
2. Caso o conteúdo possua a extensão `.lua`, a mesma será removida.
3. Se houver um caminho embutido no conteúdo, tal caminho será removido.
4. Se:
	* O resultado desse processo for uma cadeia vazia, `'_'` será atribuído ao primeiro
	índice da memória em questão.
	* Do contrário, os caracteres restantes serão analisados, da seguinte maneira:
		* Caso o primeiro caractere não seja um alfanumérico (*ou igual a `_`*), o mesmo
		será substituído por `'_'`.
		* Quanto aos demais, eles serão substituídos por `'_'` caso forem iguais a um
		caractere especial (*diferente de `'_'`*).
5. Ao fim do processo, o membro `args.flags.lib_name` receberá a cadeia resultante.

> [!NOTE]
> Mesmo se tal identificador for especificado, ele será submetido a tal validação.

#### Verificando o arquivo de saída

Após todos esses processos, caso nenhum erro ou finalização ocorra, o fluxo retornará
novamente a `check_program_arguments`, que chamará sua última função:
`void does_dest_file_already_exist(void)`.

Esta função de verificação é responsável por checar se o arquivo de destino já existe.
Em caso positivo, será verificado, por meio do membro `args.flags.replace`, se a bandeira
(`-r`) `--replace` foi utilizada, caso não, um erro fatal ocorrerá
(*informando que o arquivo já existe*), do contrário, o programa seguirá seu fluxo.

Caso a chamada sobreviva a mais essa verificação, a função `check_program_arguments` se
encerrará, fazendo o fluxo voltar para a função `main`.

#### Compartilhando valores

Agora que o fluxo retornou a `./src/main.c`, uma importante tarefa será realizada: as
informações relevantes salvas na estrutura global de `./src/args/` serão atribuídas a
estrutura global de `./src/core/`.

> [!IMPORTANT]
> Essa ação faz-se necessária por conta da arquitetura desse projeto, veja:
> [A arquitetura do projeto](#a-arquitetura-do-projeto).

Junto a esse conjunto de atribuições, o valor de `args.content_shared` será atualizado
para `true`, assim desabilitando a função de limpeza responsável por liberar a memória
alocada para a estrutura de `./src/args/`. Para mais informações sobre veja:
[O processo em si](#o-processo-em-si).

> [!NOTE]
> **VERBOSE**: Ao fim dessa tarefa, uma mensagem exibindo o estado de definição das
> bandeiras será imprimida.

## *Buffers*

Chegamos em um ponto interessante, pois a partir de agora, os sistemas voltados ao
armazenamento da saída dos algoritmos se tornará mais complexo, dado que suas saídas
se tornarão incomparavelmente mais numerosas do que aquelas geradas pelo algoritmo
anterior, de modo a que variáveis e vetores tornem-se insuficientes para tal tarefa.

Tais dados podem ser divididos em dois tipos:

1. Valores extremamente volumosos e imprevisíveis, que são (*opcionalmente*) formatados/
filtrados de modo simples (*antes de sua gravação na memória*) e exigidos apenas uma vez.

2. Dados majoritariamente curtos, mas numerosos, que necessitam ser capturados e isolados
uns dos outros. São constantemente lido e requeridos, por conta disso precisam ser
armazenados em meios rápidos, flexíveis e expansíveis.

Visto que a solução empregada a esses problemas é muito popular nos algoritmos seguintes,
faremos uma pausa na explicação linear que estava ocorrendo e adentraremos nesse
importantíssimo assunto.

> [!NOTE]
> *A explicação* continua a partir de: [Lendo o *header.lim*](#lendo-o-headerlim).

### Arquivos temporários

Grande parte das operações empregadas pelos algoritmos posteriores estarão ligadas à
leitura de arquivos persistentes, onde as informações obtidas são, muitas vezes, submetidas
a filtros e, só então, salvas no *buffer*.

Sabendo disso, organizei alguns pontos relacionados com tais dados:

* Serão salvos linearmente, ou seja, na ordem em que foram capturados.
* Não precisarão ser editadas.
* Nenhum algoritmo exigirá trechos de seu conteúdo.
* Serão exigidos apenas uma vez e como um todo.
* Exigirão um espaço dinâmico para que possam ser armazenadas.

Tendo isso em mente, pude concluir que a melhor forma de armazenar dados com estas
características seria por meio de *arquivos temporários*, pois:

* Eles expandem-se até onde o sistema operacional permitir.
* Possuem uma *API* considerável em `stdio.h`.
* São seguros quanto à memória.

> [!TIP]
> É possível criar arquivos temporários usando a função `FILE* tmpfile(void)`.

Mas: *por que não armazenar essas informações diretamente no destino?*

> A resposta é simples: certos tipos de dados tem prioridade sobre outros, ou seja, eles
> precisam ser escritos primeiros no arquivo de saída. Mais informações sobre estão
> disponíveis em: [Escopos](#escopos) e [Içamento](#içamento).

### Filas

Agora que um dos "problemas" havia sido resolvido, bastava buscar uma solução para o outro.
Este era um pouco mais complexo, já que seu dados precisariam ser gravados, lidos e obtidos
em números imprevisíveis, logo seu meio de armazenamento deveria ser rápido, expansível e
flexível (*para que pudesse ser empregado em múltiplas situações*).

Baseado nisso, bastava encontrar a melhor estrutura de dados para que tal problema desaparecesse. Após algumas ideias, cheguei a conclusão de que **filas** seriam a melhor
escolha, por conta de sua flexibilidade, lógica simples e maleabilidade.

> [!NOTE]
> Uma estrutura de *Árvore Binária AVL* foi desenvolvida, como forma de obter mais
> velocidades em cenários específicos, entretanto, por conta de sua baixa eficiência,
> ela foi removida no *commit*:
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

Começando pela estrutura, temos:

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
	* **`indet`**: **conteúdo principal**, logo **nunca será `NULL`**. Seu valor depende do
	contexto, mas, geralmente, é o identificador de uma variável, tabela ou função extraída
	do arquivo de entrada.
	* `table_key`: para o caso de `ident` ser o identificador de uma tabela *em uso*,
	tal vetor armazenará todo o conteúdo após `.` ou `:` (`foo.x`; `foo:draw()`), do
	contrário armazenará `NULL`.
	* `nick`: "versão" compactada de `ident` ou `NULL`, caso o contexto não exija 
	compactação.
* Utilitários da "mini *API*": usados pela "mini *API*" para realizar operações.
	* `quantity`: indica quantas vezes ocorreu a tentativa de reinserir `ident` em uma
	mesma fila.
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
`Queue* qee_create(char *nick, char *ident, char *table_key, bool is_const);`, e, como seu
nome dá a entender, esta é encarregada de criar um item de fila, além de inicializar seu
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
> ela não é a única, havendo também:
> ``` c
> static bool qee_add_item_status;
> static bool qee_add_item_quant_upped;
> static Queue *new_item;
> static Queue *tmp_item;
> static bool bigger_to_lower_is_allow = true;
> static IS_DUPLIC treat_duplicated_item;
> ```
> Já `IS_DUPLIC` é um tipo baseado no seguinte `enum`. Mas informações sobre estão
> disponíveis adiante.

#### Adicionando

Dando prosseguimento, falaremos agora sobre a função
`bool qee_add_item(Queue **head, char *nick, char *ident, char *table_key, bool is_const, IS_DUPLIC treat)`
e suas "irmãs": `static Queue* insert_item(Queue *item)` e
`static Queue* ordenate_queue(Queue *item)`.

Esse conjunto de funções é responsável por duas coisas:

1. Adicionar itens a uma dada fila.
2. Opcionalmente manter sua ordenação.

Partindo do início, vamos entender tal grupo de funções:

* Tudo inicia-se em `qee_add_item`, após sua chamada, onde os valores das seguintes
variáveis e ponteiro:
	* `new_item = qee_create(ident, table_key, is_const);`: *possivelmente* será
	adicionado à fila.
	* `treat_duplicated_item = treat;`: define como itens duplicados serão tratados
	(*o valor de `ident` é usado para definir quais valores são duplicados*).
	* `qee_add_item_status = false;`: valor que será retornado por `qee_add_item`.
	* `qee_add_item_quant_upped = false;`: indica se a quantidade de algum item da fila foi
	atualizada.

> [!IMPORTANT]
> `treat_duplicated_item` é uma variável do tipo `IS_DUPLIC`, baseado no seguinte `enum`:
> ``` c
> typedef enum{
> 	QEE_DROP,
> 	QEE_UP_QUANT,
> 	QEE_INSERT,
> }IS_DUPLIC;
> ```
> Onde:
> * `QEE_DROP`: indica que itens duplicados devem ser descartados.
> * `QEE_UP_QUANT`: indica que itens duplicados devem ser descartados e suas "versões" já
> presentes na fila devem ter seu membro `quantity` incrementado em `+1`.
> * `QEE_INSERT`: indica que itens duplicados devem ser adicionados a fila, após os
> similares já presentes na mesma. Não altera a quantidade de nenhum item.

* Então, a função `insert_item` é chamada `*head = insert_node(*head)`
(*seu algoritmo é simples, mas o fato de ser recursivo pode dar um nó na cabeça*):
	
	* Ela, primeiramente, verificará se seu argumento é `NULL`, retornando `new_item` caso
	verdadeira.
	
	* Após a condição anterior falhar, o valor de `bigger_to_lower_is_allow` será
	verificado, caso verdadeiro, o caso seguinte da condição será verificado, nele ocorrerá
	uma comparação entre os membros `quantity` e o comprimento dos membros `ident` do
	*item atual* e de `new_item`, que, se dada como verdadeira, resultará na inserção do
	*novo item* entre dois itens já existentes, da seguinte forma.
	
	* Seguido de uma nova falha condicional, o conteúdo presente no membro `indet`, do
	*item atual* e de `new_item`, será comparado, onde, se ambos forem iguais, um devido
	tratamento será empregado a `new_item`, baseado no valor de `treat_duplicated_item`.
	
	* Por fim, caso nenhum processo anterior finalize a função, a mesma chamará a si,
	usando o próximo item como argumento e receptor de retorno, a modo a repetir todo esse
	procedimento, até que o item seja adicionado a lista ou descartado. Após tal chamada
	recursiva, a função retornará seu próprio argumento.

* Com o fim do trabalho de `insert_item`, em `qee_add_item`, será verificado se
`bigger_to_lower_is_allow` e `qee_add_item_quant_upped` são verdadeiros em caso positivo, a
função de ordenação `ordenate_queue` será chamado. Seu objetivo é garantir que os itens presente na fila permanecerão ordenados do maior ao menor
(*mais informações na primeira nota abaixo*).

	* Inicialmente, `ordenate_queue` verificará se o *item atual* é `NULL`, caso não,
	verificará se o *próximo item* é `NULL`. Qualquer condição destas que se provar
	verdadeira resultará no retorno do *item atual*.
	
	* Após a falha da condição citada acima, ocorrerá uma comparação entre os membros
	`quantity` e o comprimento dos membros `ident` do *item atual* e de `próximo item`,
	que, se dada como verdadeira, resultará em um troca de posições, onde o
	*próximo item* tomará o lugar do *item atual*, que será enviado para frente,
	tornando-se o "novo *próximo item*". Seguidamente, o "novo *item atual*" receberá o
	retorna de outra chamada da função `ordenate_queue`, que terá o "novo *próximo item*"
	como argumento.
	
	* Ao final, caso a função não tenha sido finalizada anteriormente, `ordenate_queue`
	será chamada recursivamente, com o *próximo item* seu argumento e receptor de seu
	retorno, o que desencadeará a repetição do processo acima, até que todos os valores
	sejam estejam ordenados corretamente. Depois dessa chamada, `ordenate_queue` retornará
	seu próprio argumento.

* No final de `qee_add_item`, a mesma retornará `qee_add_item_status`, para indicar se
o novo item foi inserido na fila em questão.

> [!WARNING]
> O valor de `bigger_to_lower_is_allow` pode ser definido através do uso da função
> `void qee_bigger_to_lower(bool allow)`. Esta variável é responsável por especificar se
> a ordenação dos itens deve ser efetuada.
>
> Por padrão, os itens presentes em uma fila são ordenados daqueles com maior quantidade
> àqueles com menor, onde itens de mesma quantidade tem o comprimento de seu membro `ident`
> comparado para definir qual devem deverá ser posto primeiro. Se ambos os valores sejam
> igual, o *novo item* será posto atrás do item atual, a menos que este padrão se repita
> para eles, nesse caso, tal procedimento será repetido até que o novo item será
> devidamente tratado.

> [!IMPORTANT]
> O valor de `qee_add_item` se tornará verdadeiro sempre que um item  for introduzido à
> fila.

> [!NOTE]
> Após `insert_item` alterar a quantidade de um dado item, surgem grandes chances de sua
> nova quantidade torná-lo um item desordenado, pois `insert_item` não o realocará, caso
> isso seja necessário, pois sua habilidades para tal resumem-se ao momento da adição dele
> à fila.
> 
> Para contornar esse problema, foi criada a função `ordenate_queue`, executada após
> `insert_item`, dentro de `qee_add_item`, caso `bigger_to_lower_is_allow` será verdadeiro.

Por mais que esse algoritmo seja simples, o fato dele ser recursivo e envolver uma série de
pequenos detalhes torna sua explicação discutivelmente verbosa, entretanto isso não é bem
um problema.

#### Obtendo

Partindo agora para um algoritmo mais simples, falaremos sobre a função
`Queue* qee_get_item(Queue *item, char *ident)`. Esta função recursiva trabalha sozinha
para obter um dado item de uma determinada fila.

Sua lógica consiste em três pontos:

1. Validação: seu o item atual for igual a `NULL`, a função retornará `NULL`.
2. Comparação: se o conteúdo do seu segundo argumento for igual ao conteúdo do membro
`indet` do item atual, a função retornará o item atual.
3. Avanço: a função será chamada recursivamente, utilizando o próximo item como primeiro
argumento e mantendo o segundo.

#### Liberando

Como últimos membro dessa "mini *API*", temos as funções `void qee_free_queue(Queue *item)`
e `static void free_item(Queue *item)`, que, como é possível imaginar, são encarregadas de
liberar a memória alocada para os itens de uma fila.

Depois que o primeiro item de uma dada fila é dado como argumento a `qee_free_queue`, tal
função executará uma série de chamadas recursivas, que resultará na liberação das memórias
alocadas para `ident`, `table_key`, `nick` e para a própria estrutura do item.

> [!NOTE]
> `free_item` existe apenas por conta que seu código é utilizado em duas funções, sendo
> elas: `qee_add_item` e `qee_free_queue`.

## Lendo o *header.lim*

Agora que o fluxo retorna a `main`, estamos prestes a executar um processo de grande
importância: a verificação do *header.lim*.

> [!IMPORTANT]
> Esse tópico visa explica o funcionamento interno do algoritmo encarregado de ler e tratar
> o conteúdo e o próprio *header.lim*, para uma explicação relacionada aos seus usos,
> funcionalidades e afins, consulte:
> [O "header.lim"](https://github.com/duckafire/lim/blob/main/docs/o-header-lim.md)

Tal tarefa foi dada a função `HF_OUT_STATUS read_header_file(char **indiv_part_status)`,
armazenada em `./src/core/tools/read-header-lim.c/h`, onde tal função executa uma série de
verificações, que, se dadas como falsas, acarretam na leitura do conteúdo do *header.lim*.

> [!IMPORTANT]
> `HF_OUT_STATUS` é um tipo baseado no seguinte `enum`:
> ``` c
> typedef enum{
> 	HF_READING_DISABLE,
> 	HF_NOT_FOUND,
>	HF_IS_EMPTY,
> 	HF_CONTENT_READED,
> }HF_OUT_STATUS;
> ```
> Onde suas constantes representam os seguintes estados de saída:
> * `HF_READING_DISABLE`: indica que a leitura do *header.lim* foi desabilitada.
> * `HF_NOT_FOUND`: indica que o *header.lim* não foi encontrado.
> * `HF_IS_EMPTY`: indica que o *header.lim* foi encontrado, mas está vazio.
> * `HF_CONTENT_READED`: indica que o *header.lim* foi lido com sucesso.

Antes de adentrarmos `read_header_file`, é importante que tenhamos ciência sobre uma coisa:
tanto o processo de leitura do *header.lim* (*como um todo*), quanto a leitura de suas 
partições (*individualmente*) gera um "estado de saída".

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
ocorreu durante sua verificação. Tal valor será retornado por `read_header_file`.

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
> `fgetc`, chamada no última condição.

Como é possível imaginar, esse pequeno trecho é responsável por verificar, respectivamente:

1. Se a leitura do *header.lim* foi bloqueada.
2. Se o *header.lim* existe.
3. Se o *header.lim* está vazio.

Caso ambas falhem, isso acarretará, inevitavelmente, no retorno de `HF_CONTENT_READED`, que 
encontra-se ao fim do arquivo.

### Buscando as partições

Após tais condições, um vetor local, do tipo `HF_READ_STATUS`, será declarado, o qual é
utilizado como argumento para a função
`static void start_reading(HF_READ_STATUS *refined_status)`, que será chamada em seguida.
Tal função utilizará seu argumento para armazenar o estado de saída de todas as partições
do *header.lim*.

Ao adentrarmos em `start_reading`, podemos notar que o estado das partições é dividido em
dois: *bruto* e *refinado*. Onde:

* Estado bruto (`bool`): caso verdadeiro, indica que a próxima partição foi encontrada,
caso falso, indica que o final de *header.lim* foi encontrado. Cada partição possui seu
próprio "estado bruto".

* Estado refinado (`int`): é o estado de saída de cada partição, obtido através de um
processo que consiste na análise do "estado bruto" junto ao estado do *buffer*
(*vazio ou não vazio*), o qual responsável por armazenar os valores de cada partição.

Os "estados brutos" são preenchidos pelo retorno das funções de leitura abaixo:

* `static bool read_top_header(void)`
* `static bool read_code_scope(void)`
* `static bool read_list(Queue **buf, bool is_this_found)`

> [!NOTE]
> `read_list` é utilizada para ler *`funct list`* e *`table list`*.

Seguidamente, a função
`static HF_READ_STATUS refine_brute_status(FILE **buf, Queue **list, bool new_part_found, bool isfile)`
é utilizada para "refinar" os "estados brutos". Seu retorno é armazenado no índices do
argumento de `start_reading`.

### Aprontando as saída

Depois que `start_reading` preenche os índices de seu argumento, o fluxo do programa retorna a `read_header_file`, lá o conteúdo de tais índices será convertido (*de `int`*) 
para `char` e será armazenado no endereço apontado pelo argumento de `read_header_file`, o 
qual receberá um espaço em memória justo, destinado a tal finalidade.

Por fim, `read_header_file` retornará `HF_OUT_STATUS`, indicando que o *header.lim* foi
encontrado e lido com sucesso.
