# nobu
Yet another *__NOT__* a build system for C.

## Example
```C
#include <assert.h>
#include <stdio.h>

#include "./nobu.h"

int main(int argc, char **argv)
{
    const char *source_path = __FILE__;
    assert(argc >= 1);
    const char *binary_path = argv[0];

    if (_is_source_modified(source_path, binary_path)) {
        CMD_INIT("cc", "-o", binary_path, source_path);
        CMD_INIT(binary_path);
        exit(0);
    }

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

if we go ahead and run the binary again, it will rebuild from the new source and update the binary to that latest source

```bash
cc -o main main.c
./main

# Hello, World!
```
