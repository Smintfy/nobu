#include <stdio.h>

#include "add.h"
#include "subtract.h"

int main(void)
{
    int x = 5, y = 1;
    printf("%d + %d = %d\n", x, y, add_two_number(x, y));
    printf("%d - %d = %d\n", x, y, subtract_two_number(x, y));

    return 0;
}
