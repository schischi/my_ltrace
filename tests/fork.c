#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    pid_t child = fork();
    if (child == 0) {
        puts("Hello");
        return 0;
    }
    int status;
    waitpid(child, &status, 0);
    puts("World");
    return 0;
}
