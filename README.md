## A tiny unfinished lisp interpreter

The basis for this project were the articles by Peter Michaux (http://peter.michaux.ca/) about writing a bootstrap scheme interpreter, which source can still be found in the legacy_src directory.

The interpreter is using 
* the Boehm-Demers-Weiser garbage collector (https://github.com/ivmai/bdwgc) for memory management
* libpcre for regular expressions (http://www.pcre.org/)
* and the uthash hashtable implementation by Troy D. Hanson (http://troydhanson.github.com/uthash/) for managing the environments.


