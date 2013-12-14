#define _POSIX_C_SOURCE 1
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stddef.h>

#include "register.h"
#include "syscall.h"

void my_strace(pid_t child)
{
    /* add extra bit 0x80 to distinguish between SIGTRAP and syscall */
    ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

    while(1) {
        if (syscall_wait(child))
            break;
        syscall_print(child);
    }
}

int main(int argc, char *argv[])
{
    pid_t child;

    if (argc < 2)
        return 1;

    do {
        child = fork();
    } while (child == -1);

    if (child == 0) {
        ptrace(PTRACE_TRACEME);
        kill(getpid(), SIGSTOP);
        return execvp(argv[1], argv + 1);
    }
    else {
        waitpid(child, (int[]){0}, 0);
        my_strace(child);
    }
    return 0;
}
