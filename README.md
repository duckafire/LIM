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
--[[
		Simple example
]]

local seed = math.random(math.random(0, 65536))
_G.__CUR_MAP = 0 -- CURrent MAP

local function LIB_collision(ent1, ent2) -- ENTity
	-- reference
	local errorMsg = "Table not specified. Argument #"

	-- check arguments type
	assert(type(ent1) == "table", "1"..errorMsg)
	assert(type(ent2) == "table", "2"..errorMsg)

	-- collision between squares/rectangles
	return math.max(ent1.x, ent2.x) < math.min(ent1.x + ent1.width,  ent2.x + ent2.width ) &&
		   math.max(ent1.y, ent2.y) < math.min(ent1.y + ent1.height, ent2.y + ent2.height)
end
```

#### Compacted

``` lua
local LIB={}
do local MR19,A0,T21,MM12,MM14=math.random,assert,type,math.max,math.min local seed=MR19(MR19(0,65536))__CUR_MAP=0 LIB.collision=function(a,b)local c="Table not specified. Argument #" A0(T21(a)=="table","1"..c)A0(T21(b)=="table","2"..c)return MM12(a.x,b.x)<MM14(a.x+a.width,b.x+b.width)&&MM12(a.y,b.y)<MM14(a.y+a.height,b.y+b.height)end end
--local reference=LIB
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
