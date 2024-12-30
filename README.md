# Work in progress

<h1 id="index">Indexes</h1>

<ul>
<li><a href="#general">General vision</a></li>
	<ul>
		<li><a href="#intro">Introdution</a></li>
		<li><a href="#the-process">The compaction process</a></li>
		<ul>
			<li><a href="#proc-0">Getting and separate content</a></li>
			<ul>
				<li><a href="#proc-0-0">Identifier X Constant</a></li>
				<li><a href="#proc-0-1">Identifiers types</a></li>
			</ul>
		</ul>
	</ul>
</ul>

<br>
<hr>
<br>

<h2 id="general">General Vision</h2>

<h3 id="intro">Introdution</h3>

&emsp; The *"**L**ua L**i**brary Co**m**pactor"*, or only __*"Lim"*__, is a simple terminal program, for compact [Lua](https://lua.org "Lua website") scripts, created for easily the creation, maintain and update to libraries from [Tiny Library](https://github.com/duckafire/TinyLibrary "Repository, on GitHub"), but nothing prevents that it will be used in other contexts.

<br>

> [!IMPORTANT]
> **Lim cannot fix syntax or semantic errors in the code**, it single objective is to compact the code and mantain it working after the process, if it does not work before the compaction, it will not be work after it.

<br>

<h3 id="the-process">The compaction process</h3>

&emsp; In summary, Lim will be replace specify indetifiers to a lower string, will remove unnenessary white spaces and will group all the resultant content in **three lines**. All the process, not summaried, are listed below:

* Getting and separate content
* [Work in progress]

<br>

<h4 id="proc-0">Getting and separate content</h4>

&emsp; The the content from the file (`foo.lua`) specified is reading, it is converted in strings formed by **one** (except to `'` and `"`) special character (except `'_'`) or a sequence of alphanumeric caracters (`'_'` is included). Their classification can be divided in two parts:

<h5 id="proc-0-0">Identifier X Constant</h5>

&emsp; The diference is auto-explanatory; the strings are separated based in it first character, if it is an alphabetic character (and it is not a Lua keyword), the string is an identifier, otherwise it is a constant. Ergo: `1foo` is a constant; `"Hello_world"` is a constant; `player` is an identifier; `_car` is a constant.

<h5 id="proc-0-1">Identifiers types</h5>

&emsp; Each identifier collected are classified based in the its position (global or local; inside or outside of functions) and its _type_ (variables, table or function). It is used to choose in what buffer it will be stored and the *prefix* its "nickname". The classification "classes", and the way used to set it, are listed below:

<br>

| Type      | Variable/tables | Function | Parameters |
| :-:       | :--             | :--      | :--        |
| *Library* | Prefixed by `_G`, in declaration and ALL uses. | Declared in either place, only with prefix `function`. | |
| *Global*  | Declared in *root environment*, with prefix `local`. | Declared in *root environment*, with prefix `local function`. | Declared in *Library* or *Global functions*. |
| *Local*   | Declared in either *function environment*, with prefix `local`. | Declared in either *function environment*, with prefix `local function`. | Declared in *Local functions*. |

<br>

> [!NOTE]
> *Library* identifiers will be added to library table, because of this, they will not be compacted.

<br>
