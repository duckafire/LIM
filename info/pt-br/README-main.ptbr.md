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

## Visão geral
###### [English-US version](https://github.com/duckafire/LIM/blob/main/README.md)
&emsp; **L**ua L**i**brary Co**m**pactor (Compactador de Bibliotecas Lua) é um pequeno programa de terminal, criado para facilitar a compactação de bibliotecas Lua em um formato apelidado de [*Pacote Local*](#pacote-local). <br>

&emsp; Em resumo, Lim gera uma cópia do arquivo passado como argumento e realiza uma série de abreviações e remoções de caracteres, *tentando* não quebrar a lógica do código. Seu objetivo não é tornar o código mais rápido, mas sim menor em relação a contagem de caracteres (e em conseguentemente em `bytes`). <br>

&emsp; Lim pode ser usado em praticamente todo tipo de programa que use Lua, mas sua finalidade principal é auxiliar na construção de pequenas bibliotecas para o [Tic80 Tiny Computer](https://tic80.com "Site oficial"), especificamente bibliotecas oriundas do projeto [TinyLibrary](https;//github.com/duckafire/TinyLibrary "Repositório"). <br>

&emsp; Para obter mais informações a respeito do Lim, em relação à **regras**, boas práticas e outras informações, [*clique aqui*](https://github.com/duckafire/LIM/tree/main/info/pt-br/README-info.ptbr.md "Documentação oficial do LIM"). <br> <br>

<details><summary><b>Links úteis</b></summary>
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

## Exemplo

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
```

<br>

## Pacote local
###### Estrutura

````
~    +--> Tabela que armazenará as funções da biblioteca
~    |
~ +---------+
1 local _L={}
~
2 do local MA0=math.abs TL.abs=function(a) return MA0(a) end end
~ ++ +----------------+ +----------------------------------+ +-+
~  |    |                                           |         |
~  |    +--> Referência para funções Lua            |         |
~  |                                                |         |
~  |                        Função da biblioteca <--+         |
~  +----------------------------------------------------------+
~                 |
3 local lib=_L    +--> Contenção para o ambiente da biblioteca
~ +----------+
~      |
~      +--> Refência à biblioteca
```

<br>
