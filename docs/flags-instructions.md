# Flags instructions

* <a href="#overview">Overview</a>
* <a href="#info">Information flags</a>
	* <a href="#info-1">--version</a>
	* <a href="#info-2">--verbose</a>
	* <a href="#info-3">--help</a>
		* <a href="#info-3-1">header</a>
		* <a href="#info-3-2">list</a>
		* <a href="#info-3-3">license</a>
		* <a href="#info-3-4">list-syn</a>
		* <a href="#info-3-5">rules</a>
		* <a href="#info-3-6">synopsis</a>
* <a href="#action">Action flags</a>
	* <a href="#action-1">--replace</a>
	* <a href="#action-2">--name</a>
	* <a href="#action-3">--no-header</a>
* <a href="#syn"><i>Global</i> recommended synopsis</a>

<br>

<h3 id="overview">Overview</h3>

&emsp;Flag is the name gived to the reserved words that, when used like argument to Lim, allow turn on/off options or pass specific information. Like way of differentiate easily the flags from the other arguments, both are prefixed by `--` (*long flags*: `--flag`) or `-` (*short flags*: `-f`). They can be divided in <a href="#info">information flags</a> and <a href="#action">action flags</a>.

<br>

<h3 id="info">Information flags</h3>

<ul>
	<li id="info-1"><code>-v | --version</code>: print the program version.</li>
	<li id="info-2"><code>-V | --verbose</code>: print information about the compaction, during this process.</li>
	<li id="info-3"><code>-h | --help</code>: print information about flags and other <i>something</i>. One flag, or one of the "keywords" below, can be used like suffix (optional).</li>
	<ul>
		<li id="info-3-1"><code>h | header:</code> information about the <a href="https://github.com/duckafire/lim/blob/main/docs/the-header-lim.md" title="Official documentation"><i>header.lim</i></a>.</li>
		<li id="info-3-2"><code>l | list</code>: show all Lim flags (short and long).</li>
		<li id="info-3-3"><code>li | license</code>: show Lim <a href="https://github.com/duckafire/lim/blob/main/LICENSE" title="MIT">license</a>.</li>
		<li id="info-3-4"><code>ls | list-syn</code>: show all Lim flags (short and long), with their recommended synopsis.</li>
		<li id="info-3-5"><code>r | rules</code>: semantic rules to compaction.</li>
		<li id="info-3-6"><code>s | synopsis</code>: recommended <i>global</i> synopsis.</li>
	</ul>
</ul>

<br>

<h3 id="action">Action flags</h3>

<ul>
	<li id="action-1"><code>-r | --replace</code>: specify that, if already exist a file with the same name of the, output file must be replaced.</li>
	<li id="action-2"><code>-n | --name</code>: specify that the next argument is the output file name. One argument (output file name) is required after it.</li>
	<li id="action-3"><code>-nh | --no-header</code>: specific that the file "header.lim" must be ignored.</li>
</ul>

<br>

<h3 id="syn">Recommended <i>global</i> synopsis</h3>

<br>

```
lim [ -v | -h [arg] | <in>.lua [-r] [-V] [-nh] [-n <out>] ]
```

<br>

> [!NOTE]
> The input file need to be the first argument.

<br>
