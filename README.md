<div align="center">
    <img src="https://github.com/duckafire/LIM/blob/main/lim-icon.png" width="200"/>
</div>

<br>

<div align="center">
    <a href=""> <img alt="Last release"   src="https://img.shields.io/badge/Last%20release-v0.1.0-%2325a319" /></a>
    <a href=""> <img alt="License"        src="https://img.shields.io/badge/License-MIT-%23a61f82"           /></a>
    <a href=""> <img alt="LUA version"    src="https://img.shields.io/badge/LUA%20version-5.4-%236d1993"     /></a>
    <a href=""> <img alt="Tic80 API"      src="https://img.shields.io/badge/Tic80%20API-1.0.2164-blue"       /></a>
</div>

## Description
**Lua** L**i**brary Co**m**pactor is a small terminal program, created for to easily the compaction of LUA
libraries in a format nicknamed of "local package".

<br>

## Topic
* [More about](#moreabout)
  * [How it workd](#howitwork)
  * [*"Local package"*](#localpackage)
* [Flags](#flags)
* [Structure](#structure)
* [Rules](#rules)
* [Example](#example)
  * [Original](#original)
  * [Compacted](#compacted)
* [Useful-links](#useful-links)

<br>

## More about

#### How it work

LIM creates a copy of the LUA file passed as an argument and applies a series of rules to it. They are:

* Creation of a "mother table" for library functions.
* Protection to the strings, to the keywords, and to the names of variables, tables and functions "global"
of the library.
* Remotion of unnecessary spaces, comments, tabulations and line breaks.
* Abbreviation of the name of variables and tables "local" in the library (`name` -> `n`).
* Isolation of the library environment through the use of a ´do´ block.
* Reduced to one of the quantity of lines busy by the library.

At the end of this process, both codes will be equivalent.

#### *"Local package"*

This term refers to the fact that the final product, created by LIM, it should be implemented on file in that
it will be used, that is, libraries compacted by LIM should copied to the inside of the program files that
will used them, instead of being added to external files.

This feature of the libraries created by LIM is due to the fact that, it was created with the intention of
facilitating the compacting of libraries for the Tic80 Tiny Computer, especially the libraries present in
the project TinyLibrary. Already that the Tic80 have not have good support for external libraries, the direct
implementation of the code turns out to be the best option.

<br>

## Flags
* `-v`: Print the running version and the license.
* `-h`: Print informations about LIM.
* `-r`: Force the replacement of an already existing library, if its name is equal to the `[libName].limfile`.

<br>

## Structures
* `lim [-v; -h]`
* `lim [-r] <origin>.lua [libName]`

<br>

## Rules

1. Functions declared with `local function`, that start in the beginning of the line, they will be added to the library.
2. Variables and tables declared with `local` (or `_G.`), that start in the beginning of the line, they will not be compacted.
3. Do not create strings with `[[ ]]`.
4. Do not use `@` in code ([list of reserved words](https://github.com/duckafire/LIM/tree/main/RESERVED_WORDS.txt "All reserved words of the LIM")).
5. Variable and table names cannot finish with two numbers followed (e.g.: `car12` -> `car12_`/`car12n`/`car12th`/`...`).

<br>

## Example
#### Original
``` lua
_G.name = "Simple example"
local last

local function operation(value, Value, id, _debug)
    -- "debug" is a lua library
    if _debug then
        -- in lua interpreter
        print(value, Value, id, _debug)

        -- in tic80
        -- trace(value.."\n"..Value.."\n"..tostring(id).."\n"..tostring(debug), 4)
    end
  
    -- save last index
    if id then last = id end
  
    -- get value to id
    id = id or last or 1
  
    -- execute operation
    if id == 1 then return value + Value end
    if id == 2 then return value - Value end
    if id == 3 then return value * Value end
    if id == 4 then return value / Value end
end

--[[

    ...

]]
```

#### Compacted
``` lua
local LIB={}
do local P11=print _G.name="Simple example"local last LIB.operation=function(v,V,i,_)if _ then P11(v,V,i,_)end if i then last=i end i=i or last or 1 if i==1 then return v+V end if i==2 then return v-V end if i==3 then return v*V end if i==4 then return v/V end end end
--local reference=LIB
```

<br>

### Useful-links
* **LUA**
  * [Site](https://lua.org)
  * [GitHub](https://github.com/lua/lua)
* **Tic80**
  * [API](https://github.com/nesbox/tic-80/wiki/api)
  * [Site](https://tic80.com)
  * [GitHub](https://github.com/nesbox/TIC-80)
