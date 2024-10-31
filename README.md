# nobu
Yet another *__NOT__* a build system for C. Windows are currently not supported.

## Example
```C
#include <assert.h>
#include <stdio.h>

#include "nobu.h"

int main(int argc, char **argv)
{
    AUTO_REBUILD_SELF(argc, argv);

    printf("Hello!\n");

    return 0;
}
```

we go ahead and build this once

```bash
cc -o main main.c
./main

# Hello!
```

and now if we update or change our C program example

```C
// from printf("Hello!\n");
printf("Hello, World!\n");
```

if we go ahead and run the executable again, it will rebuild from the new source and update the binary to that latest source

```bash
./main

# Hello, World!
```
