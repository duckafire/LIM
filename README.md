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

## General vision
###### [Versão em Português-BR](https://github.com/duckafire/LIM/blob/main/info/pt-br/README-main.ptbr.md)

&emsp; **L**ua L**i**brary Co**m**pactor is a small terminal program, created for to easily the compaction of LUA libraries in a format nicknamed of [*Local Package*](#local-package). <br>

&emsp; In summary, LIM create a copy of the file used like argument and accomplish a serie of abbreviations and remotions of characters, *trying* not break the code logic. Its objective is not become the code more fast, but yes minor in relation to the characters size (and as a consequence in `bytes`)<br>

&emsp; LIM can be used in practically all type of program that use LUA, but its main finality is assist in the build of small libraries to the [Tic80 Tiny Computer](https://tic80.com "Official site"), like the libraries natives of the project [TinyLibrary](https;//github.com/duckafire/TinyLibrary "Repository"). <br>

&emsp; For obtain more informations about LIM, in relation to the **rules**, good pratices and others informations, [*click here*](https://github.com/duckafire/LIM/tree/main/info/README.md "Official Documentation of the LIM"). <br> <br>

<details><summary><b>Useful links</b></summary>
	<ul>
		<details><summary>LUA</summary>
			<ul>
				<p><a href="https://lua.org"><li> Site</li></a></p>
				<p><a href="https://github.com/lua/lua"><li>GitHub</li></a></p>
			</ul>
		</details>
		<details><summary>Tic80</summary>
			<ul>
				<p><a href="https://github.com/nesbox/tic-80/wiki/api)"><li>API</li></a></p>
				<p><a href="https://tic80.com"><li>Site</li></a></p>
				<p><a href="https://github.com/nesbox/TIC-80"><li>GitHub</li></a></p>
			</ul>
		</details>
	</ul>
</details>

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

## Local package
###### Structure

````
~    +--> Table that will be store the library functions
~    |
~ +---------+
1 local _L={}
~
2 do local P11=print _L.add=function(a,b) return a+b end end
~ ++ +-------------+ +---------------------------------+ +-+
~  |    |                                           |     |
~  |    +--> References to LUA functions            |     |                      
~  |                                                |     |
~  |                     Function of the library <--+     |
~  +------------------------------------------------------+
~                 |
3 local lib=_L    +--> Contention to the library environment
~ +----------+                   
~      |
~      +--> Reference to the library
```

<br>