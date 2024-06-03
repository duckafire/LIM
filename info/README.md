<div align="center">
    <img src="https://github.com/duckafire/LIM/blob/main/lim-icon.png" width="200"/>
</div>

<br>

<div align="center">
    <p>
   		<a href=""><img alt="Last release" src="https://img.shields.io/badge/Last%20release-v0.1.0-%2325a319"/></a>
    	<a href=""><img alt="License" src="https://img.shields.io/badge/License-MIT-%23a61f82"/></a>
    </p>
    <p>
    	<a href=""><img alt="LUA version" src="https://img.shields.io/badge/LUA%20version-5.4-%236d1993"/></a>
    	<a href=""><img alt="Tic80 API" src="https://img.shields.io/badge/Tic80%20API-1.0.2164-blue"/></a>
	</p>
</div>

## Introdution
###### [Versão em Português-BR](https://github.com/duckafire/LIM/blob/main/info/pt-br/README-info.ptbr.md)
 &emsp; LIM is a small and peculiar code compactor, created for the easy comprehension of *small* libraries written in LUA. It was through to be used in compaction of libraries of the project [TinyLibrary](https://github.com/duckafire/TinyLibrary "Repository") for the fantary computer/console [Tic80 Tiny Computer](https://tic80.com "Official site"), however, nothing prevents the same from being used to compact LUA code for other programs, like [Pico8](https://lexaloffle.com/pico-8.php "Official site") and even same [LÖVE](https://love2d.org "Official site"). <br>

> [!WARNING]\
> Currently, the LIM just has support for the Tic80 API that always considered, even through the objective is not to generate code for this program. Happily, this will change in future versions, where the read of this API will be optional. There will be the possibility of specifity reserved words, thus enlarging the support to the API for the LIM and making the compactions of LUA codes easier to vary programs.

## Topics
* [Argument structure](#argument-structure)
	* [Flags](#flags)
		* [None](#none)
		* [Version](#version)
		* [Help](#help)
		* [Replace](#replace)
	* [Argument order](#argument-order)
		* [Informative](#informative)
		* [Executive](#executive)
* [Rules to writing](#rules-to-writing)
	* [First](#first)
	* [Second](#second)
	* [Third](#third)
	* [Fourth](#fourth)
	* [Fifth](#fifth)
* [How compaction works](#how-compaction-works)
* [Good practices](#good-practices)

<br>

## Arguments structure
&emsp; Thus like any other terminal program, LIM has a series of flags and position structures for arguments, which must be followed for to execute the program. The breakdown of this organization will result in error messages.

### Flags
&emsp; This is the name given to the characters that, when prefixed by `'-'`, request the execution of a determinated action by part of the LIM. Below are listed all the flags of the LIM, together with their functionalities:

> [!NOTE]\
> `user@computer:~$` refers to information shown by terminals, like CMD and qTerminal.

<!-- ALL TEXT IN BELOW WERE NOT REVISED -->

##### None
&emsp; Case LIM will be called without arguments, this message will be shown:

```
user@computer:~$ lim
[ LIM - Lua lIbrary coMpactor - https://github.com/duckafire/LIM ]
Try: "lim -h"
```

##### Version
&emsp; Some very important information in all programs is the explanation of the version in runner, and in LIM, to obtain this information, use the flag `-v` (**v**ersion).

```
user@computer:~$ lim -v
[ LIM - vX.X.X - MIT]
```

##### Help
&emsp; Like all good terminal programs, LIM is equipped with a small help message (useful to answer simple questions), that can be summoned with the flag `-h` (**h**elp).

```
user@computer:~$ lim -h
[ LIM - Infomations ]

[!] Flags [!]
* -v : Print the running version and the current license.
* -h : Print informations about LIM.
* -r : Force the replacement of an already existig library,
       if [libName] equals its name.

[!] Structure [!]
* "lim [-v|-h]"
* "lim [-r] <origin>.lua [libName]"

[!] Rules [!]
1. Functions declared with "local function", that start in
   the beginning of the line, they will be added to the
   library. E.g.: local function name() -> lib.name=function()
2. Variables and tables declared with "local" (or "_G."),
   that start in the beginning of the line, they will not be
   compacted.
3. Do not create strings with '[[  ]]'
4. Do not ise '@' in code (reserved words).
5. Variable and table names cannot finish with two numbers
   followed (e.g.: 'car12' -> 'car12_').

[!] Full documentation [!]
* https://github.com/duckafire/LIM/tree/main/info

[ LIM - Infomations ]"
```    

##### Replacement
&emsp; Like security measures, LIM cannot replace existent files, unless the user requests it through the use of flag `-f` (**r**eplace).

```
user@computer:~$ lim -r origin.lua library
```

> [!IMPORTANT]\
> This replacement refers to the final product. The LIM does not have the capacity to manupulate the original file, only read.

### Arguments order
&emsp; LIM has its own structure in relation to the position order of its arguments, it should be followed strictly, because, if it breaks, it will result in error messages. These structures are divided into two categories:

##### Informative
&emsp; Generate a message refers to some functionality of the LIM. This structure, in general, is a compost untilization of a single flag.

```
user@computer:~$ lim [-v|-h]
```

##### Executive
&emsp; These are trigger a main functions of the LIM.

```
user@computer:~$ lim [-r] <file>.lua [libName]
```

> [!IMPORTANT]\
> Case `libName` will be omitted, `file` will be used like name for the library.

> [!IMPORTANT]\
> Arguments surrounded by `[]`, by `<>` are mandatory, and the *free* arguments should be writed of the same format that shown.

<br>

## Rules to writin
&emsp; To avoid that the LIM breaking the code logic after the compaction, a series of rules should be followed by the users during the writing of the code, like form of guide the compactor during its execution. These are listed below:

###### First
[1] **Functions declared with `local function`, that start at the beginning of the line, will be added to the library.**

* This is a small artifice used by LIM to permit that the functions of the comacted librarie can be called outside its envienment (`do ... end`), aiming to maintain certain values *private*.


``` lua
local functions example() -- [...]

------------\/------------

lib.example=function() -- [...]
```

> [!NOTE]\
> For to declarate `local` functions, in other words, functions cannot be added to the library table, it is necessary, at minimum, a tabulation on the prefix of its declaration (same line). In the future, this method will be significantly updated.

###### Second
[2] **Variables and tables declared with `local` (or `_G`), that start at the beginning of the line, will not be compacted.**

* After the compaction their names remain unchaged. This is useful for that certain values can be stored to use posteriorly and automatic by part of the library functions.

``` lua
local turnOn = false
local classes = {}

_G.libErr = ""
```

> [!IMPORTANT]\
> Variables and tables declared with `local` can only be accessed by functions internal to the library, while those *declared* with `_G` can be accessed in any stretch of the code, by *anything*.

> [!NOTE]\
> `_G` is a LUA table responsible for storing the global environment. LUA does not use this table itself, so changing its value does not affect any environment, and vice versa. It is useful for making the declaration of global variables explicit. Using `_G.` as a prefix for variables and tables declared with it (`_G.example = true`) is optional in future calls (`P11(example)`).

###### Third
[3] **Do not create strings with `[[ ]]`.**

* Like LIM groups the library into a single line, the functionality of these string declarators ends up being lost. Use `"` or `'` in alliance with `\n` to get an equal result.

``` lua
local msg0 = [[
Hello
World!
]]

local msg1 = "Hello\nWorld!"
```

###### Fourth
[4] **Do not use `@` in code.**

* LIM uses this character to make certain markings in the code, which it will guide in future actions. It was chosen because it does not have any functionality in LUA (at least not until 5.4), being limited only to use in character strings.

``` lua
local text = "@duckafire"

------------\/------------

local text = "
```

> [!WARNING]\
> Breaking this rule may result in unpredictable results.

> [!IMPORTANT]\
> In future version, the marking system of the LIM will be updated to the point of to allow the use of this character.

###### Fifth
[5] **The Variable and table names must not end with two or more numbers in a row.**

* Throughout its progress in reading the provided code, LIM leaves a series of identifiers spread throughout the code, which may conflict with variable and table names that have such characteristics.

``` lua
example01 -> example1 | example_1 | example01n | example01th
```

> [!WARNING]\
> Breaking this rule may result in unpredictable results.

> [!NOTE]\
> In future updates, the LIM guidance method will be changed, which will make it possible to discard this rule.

<br>

## How compression works
&emsp; The LIM compaction process occurs in four stages, with each of them playing a fundamental role in the functioning of the adjacent stage. They receive the following names:

1. **Definition**: Assigns the code functions, which follow the [first rule](#first), to the library table. In addition to generating *protection codes* for variables and tables.
2. **Cleaning**: Removes unnecessary spaces, comments, tabulations and line breaks. In addition to *protecting* character strings.
3. **Referencing**: Replaces LUA functions found in the code with references (variables that implicitly receive the *address* of a function).
4. **Abbreviation**: Reduces the names of variables, tables and functions to their initial letters. In addition to removing *protection codes*.

* After these processes, the generation of the final product begins, and, during it, the following are added:
	* Library table
	* `do ... end` block involving the library
	* References to LUA functions
	* Reference to the library (optional use)

<br>

## Good practices
&emsp; Given some small limitations of LIM, in relation to how minimal it can keep the code, below is a list of good habits when writing LUA codes for LIM:

[1] **Declaring variables and tables together** (*com `local`*): This habit, in the long run, allows for considerable space savings, as there will be no need to rewrite `=` and `local`. 

``` lua
local name = "player"
local damage = 37
local alive = true
local currentGun

------------\/------------

local name, damage, alive, currentGun = "player", 37, true, nil
```

> [!WARNING]\
> This is not possible with variables and tables declared with _G.

[2] **Start *compressible* variables with a letter that is not related to their name**: As previously mentioned, specifically [here](#how-compaction-works), after compression, variables and tables are reduced to their initial letters, so some attention is needed in this regard, to avoid variables with identical names. Therefore, it ends up being efficient the application of this habit.

``` lua
enemy -> aenemy | aEnemy | a_enemy
block -> bblock | bBlock | b_block
```

> [!NOTE]\
> It is recommended to follow an alphabetical order, giving the first letter to the first name.

<br>

> [**Back to top**](#introduction)