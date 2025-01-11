# The *"header.lim"*

* <a href="#overview">Overview</a>
* <a href="#file-cont">File content</a>
	* <a href="#separator">Separator string</a>
	* <a href="#buf">Buffers</a>
		* <a href="#buf-1">Top header</a>
		* <a href="#buf-2">Code scope</a>
		* <a href="#buf-3">Table list</a>
		* <a href="#buf-4">Function list</a>
	* <a href="#example">Examples</a>
* <a href="#status">Reading status</a>
	* <a href="#status-file">File</a>
	* <a href="#status-buf">Buffers</a>

<br>

<h3 id="overview">Overview</h3>

&emsp;Every that the flags `--no-header` (`-nh`) and `--until-stage` (`-us`) are not used, Lim will be to seek, in the current directory, a file named like *header.lim*. If this file is found, its content will be read and separated in memory buffers, for future processes. Else, the compaction process will be continue normally.

<br>

<h3 id="file-cont">File content</h3>

&emsp;Like way of organizate its content, the *header.lim* can contain until four partitions, each with one specific name and purpose. More information about are below:

<br>

<h4 id="separator">Separator string</h4>

&emsp;During the reading process, Lim seek, inside the *header.lim*, a specific string, named *separator string*, that indicate when the current partition end and when the next partition start. It is composited by  `\n@\n`, visually:

<br>

```

@
```

```
\n
@\n
```

<br>

* Starting from another perspective, it is possible to look this string of the next way:
	* **(1º) `\n` is a rule**: the `@` need to be writed in start line.
	* **`@` is the "separator CHAR."**: because only it is visible.
	* **(2º) `\n` is a rule**: no characters can be writed after the *"separator char."*.

<br>

<h4 id="buf">Buffers</h4>

<ul>
	<li id="buf-1"><strong>Top header</strong>: content that will be writed in top of the output file, outside of the library. Its content will not be compacted.</li>
	<li id="buf-2"><strong>Code scope</strong>: content that will be placed inside of the library, below the function scope. It is recommended that it be pre-compacted. It will not be compacted, but unnecessary white spaces will be removed.</li>
	<li id="buf-3"><strong>Table list</strong>: table list that will be putted inside the variables and tables scope, inside library. They will be searched in the <code>input.lua</code> to compact them with a specific identifier. In other words, they will be treated like Lua standard tables (<code>table</code>, <code>package</code>, ...).</li>
	<li id="buf-4"><strong>Function list</strong>: functions list that will be putted inside the function scope, inside library. They will be searched in the <code>input.lua</code> to compact them with a specific identifier. In other words, they will be treated like Lua standard functions (<code>pairs</code>, <code>type</code>, ...).</li>
</ul>

<br>

<h4 id="example">Examples</h4>

<br>

```
-- NAME: hello-world.lua
@
local function _hw()print("Hello world!")end
```

<br>

```
@
local function IS_NUM(...)for i,v in ipairs({...}) do if type(v)~="number"then 	print("#"..i.." - "..tostring(v).." is NaN")end end end
local function ERROR(c,e,f,a,i)assert(c,"ERROR [lib: foo]: "e.."; func: "..f.."; arg: "..a.." (#"..i..");")end
@
```

<br>

```
@
@
trigonometry
draw
@
printf
table_cat
delay
```

<br>

<h3 id="status">Reading status</h3>

&emsp;After the Lim tries to manipulate the *header.lim*, it genenate a *status number*, that indicates what happened during the processes. The *header.lim* and its partitions have themself *status numbers*. These codes are shown when the flag (`-V`) `--verbose` is used. Both are listed below:

<br>

<h4 id="status-file">File</h4>

| Index | Description                                                        |
| :-:   | :--                                                                |
| 0     | The reading was disable by use of the flag (`-nh`) `--no-header`   |
| 1     | The reading was disable by use of the flag (`-us`) `--until-stage` |
| 2     | The file was not found                                             |
| 3     | The file is empty                                                  |
| 4     | The file was readed with success                                   |

<br>

<h4 id="status-buf">Buffers</h4>

| Index | Description                                       |
| :-:   | :--                                               |
| 0     | The partition was not found                       |
| 1     | The partition was found, but it is empty          |
| 2     | The partition was found and it have valid content |

<br>

> [!NOTE]
> The buffers status are returned together (`1100`; `2212`; `0000`), in order: *top header*, *code scope*, *table list* and *function list*.

<br>

> [!NOTE]
> Only the *top header* not ignore white space.

<br>

> [!NOTE]
> *table list* and *function list* ignore special characters (except `'_'`).

<br>
