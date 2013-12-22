#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main()
{
    int len;
    int len2;
    char *foo = strdup("abc");
    char *bar = strdup("def");
    len = strlen(foo);
    len2 = strlen(bar);
    printf("Len = %d\n", len);
    printf("Len = %d\n", len2);
    return 0;
}
