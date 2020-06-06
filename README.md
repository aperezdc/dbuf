dbuf
====

[![builds.sr.ht status](https://builds.sr.ht/~aperezdc/dbuf/alpine.yml.svg)](https://builds.sr.ht/~aperezdc/dbuf/alpine.yml?)

Installation
------------


With [clib](https://github.com/clibs/clib):

```sh
clib install aperezdc/dbuf
```

Example
-------

```c
#include "dbuf.h"

int
main(int argc, char **argv)
{
    struct dbuf b = DBUF_INIT;

    dbuf_addstr(&b, "Command line arguments:\n");

    for (int i = 1; i < argc; i++) {
        dbuf_addstr(&b, argv[i]);
        dbuf_addch(&b, '\n');
    }

    puts(buf_str(&b));
    dbuf_clear(&b);

    return EXIT_SUCCESS;
}
```
