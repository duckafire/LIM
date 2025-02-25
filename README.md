<div align="center"><img src="https://github.com/duckafire/LIM/blob/main/lim-icon.png" width="200"/></div>
<div align="center">
    <p>
   		<a href=""><img alt="Last release" src="https://img.shields.io/badge/Last%20release-v1.0.0-%2325a319"/></a>
    	<a href=""><img alt="LUA version" src="https://img.shields.io/badge/LUA%20version-5.3-blue"/></a>
	</p>
</div>

[lua]: https://lua.org "Lua website"
[tinylibrary]: https://github.com/duckafire/TinyLibrary "Repository, in GitHub"
[github-issues]: https://github.com/duckafire/LIM/issues "Github issues"

## Indexes

* [Introduction](#introduction)
* [Compaction process](#compaction-process)
* [Basic commands](#basic-commands)

> [!TIP]
> [Read the documentation](https://github.com/duckafire/LIM/blob/main/docs/README.md).

---

### Introduction

The Lua L**i**brary Co**m**pactor, or only **Lim**, is a simple terminal program to
compact [Lua][lua] scripts. It was created for easily the creation, maintain and update of
libraries from [Tiny Library][tinylibrary], but nothing prevent that it will be used in
other contexts.

> [!IMPORTANT]
> **Lim cannot fix syntax and semantic errors in the code**, it single objective is to
> compact the code and maintain it is working after of the process. If it does not work
> before the compaction, it will not work after it.

---

### Compaction process

**In summary**:

1. Lim will replace the identifiers, from input file, to a lower identifier (`foo -> a`).
2. It will remove unnecessary white spaces.
3. And it will group all the *result* content in a block `do end`, positioned in
**one line**.

In addition to the main line, there are other two lines, one above (with the declaration
of the *core table*) and other below (with the declaration of the *reference table*). See
this example:

> Before compaction:

``` lua
function foo()
	return "foo"
end
```

> After compaction:

``` lua
local _={}
do function _.foo()return"foo"end end
local lib=_
```

---

### Basic commands

```
# compact a file
lim <input>.lua [--replace] [--name <output>]

# asking for help
lim --help [flag-name | help-argument]

# examples
lim foo.lua
lim foo1.lua --name FOO-1
lim --help
lim --help --replace
```

> [!NOTE]
> Please report bugs [here][github-issues].
