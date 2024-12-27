# Index

<ul>
<li><a href="#architecture">Architecture Pattern</a></li>
<li><a href="#flag-list"   >Flag list</a></li>
<li><a href="#arg-to-help" >Argument to flag <code>help</code></a></li>
<li><a href="#error-list"  >Error code list</a></li>
<!-- <li><a href="#"></a></li> -->
<ul>

<br>
<hr>

<h2 id="architecture">Architecture Pattern:</h2>
<h4>Master-slave</h4>

* master: does requisition the all other partitions and receive their returns.
	* args: argument treatment (flag vality; on/off compact options; print text on screen).
		* print: texts to error and information flags (e.g. `help` and `verbose`).
		* rules: check all arguments (invalid => error) and on/off flag options (e.g. `replace` and `until stage`).

<br>
<hr>

<h2 id="flag-list">Flag list</h2>

* -h  | --help
* -n  | --name
* -nh | --no-header
* -r  | --replace
* -us | --until-stage
* -V  | --verbose
* -v  | --version

<br>
<hr>

<h2 id="arg-to-help">Arguments to flag <code>help</code></h2>

* h  | header
* li | license
* l  | list
* ls | list-syn
* r  | rules
* s  | synopsis

<br>
<hr>

<h2 id="error-list">Error code list</h2>

| Code | Cause                           |
| :-:  | :--                             |
| 0    | Success                         |
| 1    | Source file not exist           |
| 2    | Flag repeatition                |
| 3    | Suffix expected after a flag    |
| 4    | Invalid suffix to flag          |
| 5    | Invalid argument to flag `help` |

<br>
<hr>
