<br>

---

__*WORK IN PROGRESS*__

---

<br>

# LIM
#### Lua lIbraries coMpactor - v0.0.0
##### Using [LUA](https://github.com/lua/lua "LUA Repository") **5.4**
It is a small terminal program, created to compact libraries of the TinyLibrary.
This tool can be used to compact `lua` libraries to use on Tic80 or other fantary consoles, like `pico8`.

## Topic
* [Flags](#flags)
* [Structure](#structure)
* [Rules](#rules)
* [Example](#example)

<br>

## Flags
* `-v`: Print the running version and the current license.
* `-h`: Print informations about LIM. 
* `-r`: Force the replacement of an already existing library if `[libName]` equals its name.

<br>

## Structure
* `lim [-v; -h]`
* `lim [-r] <origin>.lua` [libName]

<br>

## Rules

1. Functions declared with `local function`, that start in the beginning of the line, they will be added to the library.
2. Variables and tables declared with `local` (or `_G.`), that start in the beginning of the line, they will not be compacted.
3. All words (except the reserved) prefixed by tabulation and space, they will be compacted.
4. Do not create strings with ´[[ ]]´.
5. Do not use ´@´ in code ([reserved word](https://github.com/duckafire/LIM/tree/main/RESERVED_WORDS.txt "All reserved words of the LIM")).

<br>

## Example
``` lua
local function Add(a, b)
    -- it can be used to
    -- calcule subtract
    return a + b
end
```

``` lua
lib.add(a,b) return a+b end
```

<br>

##### Repositories
* [Tic80](https://github.com/nesbox/TIC-80)
* [TinyLibrary](https://github.com/duckafire/TIN)

<br>
