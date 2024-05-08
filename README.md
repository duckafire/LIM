<br>

---

__*WORK IN PROGRESS*__

---

<br>

###### v0.1.1
# tin
It is a small terminal program, created to compact libraries of the TinyLibrary.
This tool can be used to compact `lua` libraries to use on Tic80 or other fantary consoles, like `pico8`.

## Topic
* [Example](#example)
* [Rules](#rules)
* [Commands](commands)

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


## Rules
* Start a library function like `local function`.
* Start a function name with an uppercase letter.
* Do not write lines with more than 1,000 characters.
* Do not use tabulation or space before `local function`.
* Do not use more than one space between the words `local function Name`.

<br>

## Commands

* tin --help
* tin <origin>.lua <libName>
* tin <origin>.lua <libName> --repl

<br>

##### Repositories
* [Tic80](https://github.com/nesbox/TIC-80)
* [TinyLibrary](https://github.com/duckafire/TIN)

<br>
