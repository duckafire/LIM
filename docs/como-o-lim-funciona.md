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
