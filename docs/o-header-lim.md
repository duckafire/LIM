# O "*header.lim*"

---

> [!IMPORTANT]
> Essa página não se propõem a explicar como ocorrer o processo de leitura do *header.lim*,
> seu objetivo é explicar *o que é o header.lim*. Para obter informações sobre o
> funcionamento desses processos, acesse: [Como o Lim funciona?: Leitura do *header.lim*
](https://github.com/duckafire/lim/blob/main/docs/como-o-lim-funciona.md#leitura-do-headerlim).

---

* [Visão geral](#visão-geral)
* [Conteúdo do arquivo](#conteúdo-do-arquivo)
	* [Separator de partições](separador-de-partições)
	* [Partições](#partições)
		* *Top header*
		* *Code scope*
		* *Table list*
		* *Funct list*
	* [Exemplos](#exemplos)
* [Estado de leitura](#estado-de-leitura)
	* [Arquivo](#arquivo)
	* [Partições](#partições)

---

## Visão geral

Sempre que a bandeira `--no-header` (`-nh`) não é utilizada, Lim buscará, no diretório
atual, por um arquivo nomeado como *header.lim*. Se esse arquivo for encontrado, seu
conteúdo será lido e separado em *buffers*, para futuros processos. Caso contrário, o
processo de compactação continuará normalmente.

---

## Conteúdo do arquivo

Como forma de organizar seu conteúdo, *header.lim* é dividido em até quatro partições, onde
cada uma delas possui um nome e propósito específico para seu conteúdo.

### Separador de partições

Durante o processo de leitura, Lim buscará, dentro do *header.lim*, por uma cadeia de
caracteres específica, chamada de *Cadeia separadora*. Ela é resposável por indicar o fim
da partição atual e início da próxima. Sendo composta por `\n@\n`, visualmente:

```

@
```

```
\n
@\n
```

* Partindo de outra pespectiva, é possível ver essa cadeia da seguinte maneira:
	* **1º é uma regra**: o `@` deve ser escrito no início da linha.
	* **`@` é o** ***caractere de separação***: pois apenas ele é visível.
	* **2º é uma regra**: nenhum caractere deve ser escrito após o *caractere separador*.

### Partições

* ***Top header***: conteúdo que será escrito no topo do arquivo de saída, acima da
biblioteca. Seu conteúdo não será alterado.

* ***Code scope***: conteúdo que será posicionado dentro da biblioteca, abaixo do *escopo
de referências de funções*. Seus identificadores não serão comprimidos, mas espaço em
branco desnecessários serão removidos, os necessário serão susbtituído por um caractere
de espaço.

* ***Table list***: lista de identificadores (de tabelas) que receberão o mesmo tratamento
que as tabelas do *Padrão Lua*.

* ***Funct list***: lista de identificadores (de funções) que receberão o mesmo tratamento
que as funções do *Padrão Lua*.

## Exemplos

```
-- NAME: hello-world.lua

@
local function _hw()print("Hello world!")end
```

```

@
local function _add(a,b)return a+b end
local function _sub(a,b)return a-b end
local function _mul(a,b)return a*b end
local function _div(a,b)return a/b end
local function _fmo(a,b)return a%b end
local function _pow(a,b)return a^b end
@
```

```

@

@
screen
draw
@
printf
tabcat
delay
```

> [!CAUNTION]
> Sempre deixe uma quebra de linha em partições vazias.

---

## Estado de leitura

Depois que o Lim tenta manipular o *header.lim*, ele gera um *código de estado*, que indica
o que ocorreu durante seu processamento. Tanto o *header.lim*, quanto suas partições
possuem seus próprios *códigos de estado*. Eles serão exibidos no console apenas caso a
bandeira `--verbose` (`-V`) seja usada.

### Arquivo

| Código | Descrição                      |
| :-:    | :--                            |
| 0      | Leitura desabilitada por `-nh` |
| 1      | Arquivo não encontrado         |
| 2      | Leitura realizada com sucesso  |

### Partições

| Código | Descrição                 |
| :-:    | :--                       |
| 0      | Partição não encontrada   |
| 1      | A partição está vazia     |
| 2      | Partição lida com sucesso |

> [!NOTE]
> Os *códigos de estado* da partições são retornados juntos (`1100`; `2212`; `0000`).
