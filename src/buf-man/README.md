### Binary tree
* id: first character of the *content[1]*.
* quantity: increase in one, when a node with same `id` and `content[1]` was found.
* content[0]: table (identifier from *Lua* or from `header.lim`), nickname or `NULL`.
* content[1]: variables, tables, functions or parameter identifier.
* left: node with a lesser `id`.
* right: node with a bigger `id`.
* next: node with a same `id`, but a different `content[1]`

### Queue
* id: first character of the *content[1]*.
* quantity: increase in one, when a item with same `id` and `content[1]` was found.
* content[0]: table (identifier from *Lua* or from `header.lim`), nickname or `NULL`.
* content[1]: variables, tables, functions or parameter identifier.
* next: item with a lesser `quantity` or with a lesser length of `content[1]`.

### *source-buffers.c*
&emsp;Store the content getted from source file, classified in:
* **library function**: declared in some "environment", without the prefix `local`. They will be added to library table.
* **library variable/table**: prefixed by *Lua* table `_G`. They will be added to library table. `_G` will be replaced by `_`.
* **"private global" function**: declared in "root environment", with `local`.
* **"private global" variable/table**: declared in "root environment", with `local`.
* **table and function from *Lua* standard**: like `math`, `type` and `table`.
* **functions from `header.lim`**: functions specififed in `header.lim`, in partition list (#3).
* **use or call**: variables, tables, paramaters and function used after their declarations.
* **constants (anonymous functions included)**: content that will not be compacted, like numbers, table keys and string.
* **local function**: declared inside function enviroment, with prefix `local`.
* **local variables/table**: declared inside function enviroment, with prefix `local`.
* **parameters to "root function"**: declared in "root functions".
* **parameters to function inside functions**: declared in function aligned with "root functions".
