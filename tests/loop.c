#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char *foo = strdup("bar");
    while (1) {
        printf("%lu\n", strlen(foo));
    }
    return 0;
}
