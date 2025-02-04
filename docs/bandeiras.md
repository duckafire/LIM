# Bandeiras

---

* [Visão geral](#visão-geral)
* [Bandeiras informativas](#bandeiras-informativas)
	* `--help` - `--verbose`
	* `--version`
* [Bandeiras de ação](#bandeiras-de-ação)
	* `--lib-name` - `--name`
	* `--no-header` - `--replace`
* [Sinopse global recomendada](#sinopse-global-recomendada)

---

## Visão geral

Bandeiras (em inglês *flag*), no contexto desse projeto, é o nome dado a palavras
chaves que podem, opcionalmente, ser usadas como argumento para o Lim. Elas permitem
obter informações, ativar/desativar funcionalidades ou informar "valores" específicos,
assim possibilitando um processo customizado.

Cada um desses argumentos especiais está disponível em duas versões: *longa* e
*curta*. A diferença está unicamente na escrita, onde a *versão longa* é prefixada por
`--` e tem seu nome como sendo uma referência a sua função, já a *versão curta* é prefixada
por `-` e é formada pela(s) inicial(is) da *versão longa* (podendo ser minúscula ou
maiúscula).

> [!NOTE]
> Algumas bandeiras podem exigir um sufixo (que será usado para aplicar alguma 
> configuração) e/ou possuir suas próprias palavras reservadas, como é o caso de
> `--help`.

---

## Bandeiras informativas

Como o própio nome sugere, essas bandeiras servem unicamente para imprimir
informações no console, sendo estas referentes ao Lim e aos seus processos.

* `-h | --help`: gera uma mensagem informativa com base no seu sufixo opcional, que pode
ser **qualquer** bandeira ou uma das palavras reservadas abaixo.

	* `h  | header`: exibe informações essenciais sobre o *header.lim*. Veja mais sobre em:
	[O *header.lim*](https://github.com/duckafire/blob/main/docs/o-header-lim.md).

	* `l  | list`: lista todas as bandeiras do Lim.
	
	* `li | license`: imprime a licença de uso, modificação e distribuição deste projeto
	(que é a [MIT](https://github.com/duckafire/lim/blob/main/LICENSE)).
	
	* `ls | list-syn`: lista todas as bandeiras do Lim, junto a sua sinopse recomendada.

	* `r  | rules`: exibe os padrões/regras semâticos do Lim, usados para certas
	definições. Veja também:
	[Regras semânticas](https://github.com/duckafire/blob/main/docs/regras-semanticas.md).

	* `s  | synopsis`: exibe a sinopse recomendada para o posicionamento dos todos os
	argumentos e bandeiras possíveis, simultaneamente. O mesmo que esta disponível
	[abaixo](#sinopse-global-recomendada).

* `-V | --verbose`: exibirá informações relacionadas aos processos que ocorrem durante a
compactação, simultaneamente a ela.

* `-v | --version`: exibe a versão do Lim.

---

## Bandeiras de ação

Há também as bandeiras responsável por permitir a ativação/desativação de recursos
ou espeficicação de "componentes", de modo a modificar o "produto final".

* `-ln | --lib-name`: permite informar qual será o nome da *tabela de referência*,
responsável por armazenar a *tabela núcleo* da biblioteca. Tal identificador deve ser
seu sufixo. Caso essa bandeira não seja especificada, o nome do arquivo fonte será
utilizada para gerar o nome da biblioteca (*tanto este, quanto o valor especificado
através desta bandeira passarão por um processo de validação*).

* `-n | --name` permite especificar o nome e caminho de destino do arquivo de saída. Tal
nome/caminho dever ser sufixo à esta bandeira. Por padrão, o arquivo de saída terá o
mesmo nome que o arquivo original, com a extensão `.lim` (ao invés de `.lua`), e
será posto no diretório atual.

* `-nh | --no-header`: especifica que o arquivo `header.lim` não dever ser processado.
Veja mais sobre em: [O *header.lim*](https://github.com/duckafire/blob/main/docs/o-header-lim.md)

* `-r | --replace`: especifica que, caso já exista um arquivo com o mesmo nome do arquivo
de saída, este deverá ser substituído. Se está bandeira não for especificada e houver um
arquivo com tal característica, Lim gerará um erro.

---

## Sinopse global recomendada

Como é de se imaginar, Lim espera que seus argumentos sigam uma certa ordem lógica,
sejam eles bandeiras ou não. Entretanto, há uma certa flexibilidade em alguns aspectos,
permitindo que certos argumentos possam ser posicionados em lugares diversos, o que pode
gerar uma certa confusão. Como forma de orientar os usuários, foi criada uma sinopse
global (opcional, mas recomendada), a qual encontra-se abaixo:

```
lim [--version | --help [keyword | flag] | <input-file>.lua [--replace]
    [--no-header] [--name <output-file-name>] [--lib-name <identifier>] ]

```
