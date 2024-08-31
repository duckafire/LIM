<h2 id="0">Topics</h2>

###### [Versão em Português-BR](https://github.com/duckafire/LIM/blob/main/info/pt-br/README-info.ptbr.md)

* [Description](#1)
	* [Operation](#1-1)
	* [*TinyLibrary*](#1-2)
* [Command line](#2)
	* [Lines](#2-1)
	* [Structures](#2-2)
	* [Final product](#2-3)
* [Rules](#3)
	* [Functions declaration](#3-1)
	* [Variables and tables declaration](#3-2)
	* [Strings creation](#3-3)
	* [Reserved character](#3-4)
	* [Last characters](#3-5)
* [Known issues](#4)
	* [Occasional *garbage*](#4-1)
	* [Unsupported path](#4-2)
	* [Break in hexadecimal values](#4-3)
	* [Incorrect compaction of *global*](#4-4)
	* ["Limitation" for local values](#4-5)
		
<br>

<h2 id="1">Description</h2>

&emsp; **L**ua L**i**brary Co**m**pactor, or only **Lim**, is a terminal program created with the intention of to facilitate the compaction of code written in [`Lua`](https://lua.com "Site oficial da linguagem") para use no project [*TinyLibrary*](#1-2) (but it can be used in any other program that use `Lua`), in order to maintain the operation of the code and exporting it in a format named like *Local Package*.

<h3 id="1-1">Operation</h3>

&emsp; The compaction process do Lim in relatively simple and consist in the renomination of the variable, tables and functions names *unprotected* (and created by user) by smaller names, *usually* formed by only one character.

``` lua
-- example
local function add(value1, value2)
	return value1 + value2
end
```

``` lua
local function add(a,b) return a+b end
```

&emsp; Besides this renomination, in such a process, names of Lua native functions (like: `type`) are replaced by a *code*, which is nothing more than a variable that references the function in question, but with a minor quantity of characters in its name, which, in the long term, makes a considerable difference in code size.

| Original | Reference |
| :--      | :--       |
|error("Message")<br>type(false)<br>print("Hello World"!) | E3("Message")<br>T21(false)<br>P11("Hello World!") |

&emsp; After this process, not everything will be compacted, because such an act would generate the break of the syntactic and semantic logic of the code. The exception to the compaction applies to:

* *Environment* global: that only can be accessed and manipulated inside the library environment and by functions in it.
* *Pure* global: that can be accessed and manipulated by any code present in the program (that has been written after the library *implementation*).
* *Internal* functions: only can be called inside the library *environment*.
* Library functions: are indexed to the library table, can be called outside its *environment*.

###### See too: [Rules](#3)

<h4 id="1-2">TinyLibrary</h4>

&emsp; This is the name given to the library collection for the fantasy console/computer [Tic80 Tiny Computer](https://tic80.com "Official site"), which aims to abstraction several resources from some and availability of utility codes.

&emsp; A big particularity of the Tic80 is your character limit (`65.536` to base version and `24.288` in `PRO version`), which resulted in creation of the *compacted format/version* for the library of such collection, so that users had more space to write the code of its project.

&emsp; Initially, this process was manual, but with the growth of library codes, it became   the creation of an auxiliary tool, so that obtain a bigger efficiency.

<br>

> [!IMPORTANT]\
> Currently, the support that the Lim has to the Tic80 cannot be turn offed, that is, codes provided to it, that not will be used in Tic80, do not have identical names to the [API](https://github.com/nesbox/TIC-80/wiki/API "Tic80 API - Wiki") of such program. Happily, in future updates, this support will become optional.

<br>

###### [See more](https://github.com/duckafire/TinyLibrary "TinyLibrary official repository")

<br>

<h2 id="2">Command line</h2>

&emsp; Just like every terminal program, Lim has some *flags* and organization structures, that must be used to execute functions and customize the result of some.

<h4 id="2-1"> Flags</h4>

* `-v`: Print the Lim version in execution and its license.
* `-h`: Print several informations about Lim, like flags and [rules](#3).
* `-r`: Force the file replacement, case already exist a file with some name of the [final product](#2-1).

<br>

> [!NOTE]\
> `-r` do not replace the origin file. 

<br>

> [!NOTE]\
> Case `libName` is not specified, `origin` will be used in its place.

<br>

<h4 id="2-2">Structures</h4>

* `lim [-h|-v]`
* `lim [-r] <origin>.lua [<libName>]`

<br>

> [!TIP]\
> Usually, in code structures to terminal programs, words between `[ ]` are optional and between `< >` are words those spelling may vary. Already, the`|` ("*or*") indicate the possibility of to choose between the use of its neighboring options.

<br>

##### For example

* `lim library.lua`
* `lim -r animation.lua AN`

<br>

> [!TIP]\
> Specify smaller name to `libName` (`AN` in example), to save more space in the code, and after, optional, rename the file, to that it can be easier to identify and manipulate it (`AN.limfile` -> `animation.limfile`).

<br>

> [!IMPORTANT]\
> Do not specify a [reserved word](https:///github.com/duckafire/LIM/blob/main/info/RESERVED_WORDS.txt "Lista de palavras reservadas do Lim") to `libName`.

<br>

<h4 id="2-3">Final product</h4>

&emsp; Final product, library, compacted file ([...]) are some of the denomination given to the files generated by success of the compaction, this being a text file with the extension `limfile`. Such file will contain all compacted content, ready to *copy and paste*.

&emsp; Its structure is below:

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

<br>

> [!TIP]\
> The use of a reference is not mandatory, but it is recommended, already that possibility a bigger save of space, especially when then library in question had a big name, like [`Magic_Palette`](https://github.com/duckafire/TinyLibrary/tree/main/collection/compacted "Library of the TinyLibrary").

<br>

<h2 id="3">Rules</h2>

&emsp; In a way of guiding the Lim during the compaction, it is necessary to follow a series of semantic rules, which aim (in addition to guiding Lim) to explain the way in which certain values will be treats and avoid *breaking*. Both are listed below:

<h4 id="3-1">1 - Functions with name prefixed by <code>LIB_</code> (in declaration and calls) will be added to library table.</h4>

&emsp; This way, it is clear, both that the Lim when for the developer, what functions do they use resources from library itself.

<br>

> [!NOTE]\
> After the compaction, `LIB_` will be replaced by `libName`.

<br>

<h4 id="3-2">2 - Variables and tables with the prefix <code>local</code> (or <a href="https://www.lua.org/manual/5.3/manual.html#pdf-_G"><code>_G</code></a>) and declared in start of the line, will not be compacted.</h4>

&emsp; This was thought to make persistent storage possible of values during the program execution, so what library functions can manipulate it and provide it to users.

<br>

> [!IMPORTANT]\
> Functions declared in start of the lines, that it is not prefixed by `LIB_`, also it will not be compacted.

<br>

> [!NOTE]\
> Variables and tables created with `local` only can be accessed by functions inside library *environment* (mas they can be accessed through the use of functions), already those created with [`_G`](https://www.lua.org/manual/5.3/manual.html#pdf-_G "Lua 5.3 official documentation") can be accessed with any code line posteriorly to the library.

<br>

> [!TIP]\
> Declared multiple variables using a single reserved word (`local`), as follows: `local values, effect, code = {}, false, tonumber(0x03FF8)`.

<br>

<h4 id="3-3">3 - Do not create strings with <code>[[ ]]</code>.</h4>

&emsp; The main functionality of this delimiter is its capacity to converted line breaks in inside it in lines breaks to the string in question, soon, like after the compaction realized by Lim a library becomes occupying only one line, this delimiter loses its differential.

<br>

> [!IMPORTANT]\
> Lim do not respect strings delimited by them (`[[ ]]`).

<br>

> [!TIP]\
> Use the [Space character](https://en.m.wikipedia.org/wiki/Escape_sequences_in_C "Página da Wikipédia sobre Caracteres de Escape") `\n` to break line(s) in your strings.

<br>

<h4 id="3-4">4 - Do not use <code>@</code> in code.</h4>

&emsp; During the compaction process, this character is used to leave several marks in code, so that Lim can orient itself in future actions. That said, it is possible to consider this as a [reserved "word"](https:///github.com/duckafire/LIM/blob/main/info/RESERVED_WORDS.txt "List of Lim reserved words").

<br>

> [!IMPORTANT]\
> Its use *can* lead to unpredictable result and problems. 

<br>

> [!NOTE]\
> In future updates, this marks system of the Lim will be refined, what solved all e any problem that the use of this character can generate.

<br>

<h4 id="3-5">5 - Do not finish functions, variables and tables names with two or more numbers followed.</h4>

&emsp; Names that break this rule will be considered, by Lim, as number.

<br>

<h2 id="4">Known issues</h4>

&emsp; This version (`0.1.11`) of Lim in many stable and usable, but it is at mercy to some problems, which generate *faulty* codes, but that happily can be fixed in easy manner and (often) fast (but only manually).

<h4 id="4-1">1 - Sometimes trash is generated near the end of the code.</h4>

&emsp; There is a way to stop this trash created, but he is not 100% efficiency, then, eventually, some trash can might end up in the [final product](#2-3). This trash consist of a few (0-15) random characters.

<h4 id="4-2">2 - File path are not supported, only the file name.</h4>

&emsp; That is, case `origin` is specified like `src/origin.lua` it will be denoted like invalid, in this case, so it is necessary to be *inside* the file directory in question.

<h4 id="4-3">3 - Hexadecimals values are breaking in compaction process.</h4>

&emsp; In other words, such numeric codes are not compacted, but they lose part of its characters, what generate a considerable break in code logic.

<h4 id="4-4">Incorrect compaction of *global*.</h4>

&emsp; When creating two or more *global* variables or tables using only one `local` (`local var1, ..., varn`), only the first identifier will be spared of the compaction (`local var1, var2` -> `local var1, a`).

<h4 id="4-5">"Limitation" for local values.</h4>

&emsp; Case one `mother function` contain more of 26 identifier, the 27th will be fomated by 'special characteres (`{`, `|`, ...) + one number + the original identifier', in other words, it will be a invalid name to them.

<br>

> [!TIP]\
> `local var = tonumber(0x00)` or `local var = string.format("%x", "0x00")` can be used to around the 3st problem.

<br>

> [!IMPORTANT]\
> Future version of the Lim will bring corrections to both problems above.

<br>

### [Come back to top](#0)
