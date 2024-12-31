<div align="center"><img src="https://github.com/duckafire/LIM/blob/main/lim-icon.png" width="200"/></div>
<div align="center">
    <p>
   		<a href=""><img alt="Last release" src="https://img.shields.io/badge/Last%20release-v0.1.0-%2325a319"/></a>
    	<a href=""><img alt="LUA version" src="https://img.shields.io/badge/LUA%20version-5.3-blue"/></a>
	</p>
</div>

# Indexes

###### Work in progress

* [Introduction](#introduction)
* [Compaction process](#compaction-process)
* [Basic commands](#basic-commands)
* [Known bugs](#known-bugs)

<br>

> [!TIP]
> [Read the documentation.](https://github.com/duckafire/LIM/blob/main/docs.md "Work in progress")

<br>

<hr>

# Introduction

&emsp;The **L**ua L**i**brary Co**m**pactor, or only **Lim**, is a simple terminal program, to compact [Lua](https://lua.org "Lua website") scripts. It was created for easily the creation, maintain and update of libraries from [Tiny Library](https://github.com/duckafire/TinyLibrary "Repository, in GitHub"), but nothing prevent that it will be used in other contexts.

<br>

> [!IMPORTANT]
> **Lim cannot fix syntax and semantic errors in the code**, it single objective is to compact the code and mantain it working after the process, if it does not work before the compaction, it will not be work after it.

<br>

# Compaction process

&emsp;In summary, Lim will replace the identifiers to a lower string (`foo -> f`), will remove unnecessary white spaces and will group all the result content in a block `do end`, positioned in **one line**. In addition to the main line, there are other two, one above (with the declaration of the *library table*) and other below (with the declaration of the library name). Like this:

<br>

``` lua
local _={}
do _.foo=function() return "foo" end end
local libfoo=_
```

<br>

# Basic commands

```
# compact a file
lim file-name.lua [--replace] [--name \<output-file-name>]

# asking for help
lim --help [flag-name | help-argument]

# examples
lim foo.lua
lim foo1.lua --name FOO-1
lim --help
lim --help --replace
```

<br>

# Known bugs

&emsp;For now, this list is empty :D

<br>
