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
###### [English version](https://github.com/duckafire/LIM/blob/main/README.md)
&emsp; **L**ua L**i**brary Co**m**pactor (Compactador de Bibliotecas LUA) é um pequeno programa de terminal, criado para facilitar a compactação de bibliotecas LUA em um formato apelidado de [*Pacote Local*](#pacote-local). <br>

&emsp; Em resumo, LIM gera uma cópia do arquivo passado como argumento e realiza uma série de abreviações e remoções de caracteres, *tentando* não quebrar a lógica do código. Seu objetivo não é tornar o código mais rápido, mas sim menor em relação a contagem de caracteres (e em conseguentemente em `bytes`). <br>

&emsp; LIM pode ser usado em praticamente todo tipo de programa que use LUA, mas sua finalidade principal é auxiliar na construção de pequenas bibliotecas para [Tic80 Tiny Computer](https://tic80.com "Site oficial"), especificamente bibliotecas oriundas do projeto [TinyLibrary](https;//github.com/duckafire/TinyLibrary "Repositório"). <br>

&emsp; Para obter mais informações a respeito do LIM, em relação à **regras**, boas práticas e outras informações, [*clique aqui*](https://github.com/duckafire/LIM/tree/main/info/pt-br/README-info.ptbr.md "Documentação oficial do LIM"). <br> <br>

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

#### Compactado

``` lua
local LIB={}
do local P11=print _G.name="Simple example"local last LIB.operation=function(v,V,i,_)if _ then P11(v,V,i,_)end if i then last=i end i=i or last or 1 if i==1 then return v+V end if i==2 then return v-V end if i==3 then return v*V end if i==4 then return v/V end end end
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
2 do local P11=print _L.add=function(a,b) return a+b end end
~ ++ +-------------+ +---------------------------------+ +-+
~  |    |                                           |     |
~  |    +--> Referência para funções LUA            |     |                      
~  |                                                |     |
~  |                        Função da biblioteca <--+     |
~  +------------------------------------------------------+
~                 |
3 local lib=_L    +--> Contenção para o ambiente da biblioteca
~ +----------+                   
~      |
~      +--> Refência à biblioteca
```

<br>