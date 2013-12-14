#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stddef.h>

#include "strace.h"
#include "register.h"
#include "syscall.h"

static const char *syscalls[] = {
#define X(Name, Num) #Name,
#include "syscalls.def"
#undef X
};

void syscall_print(pid_t child)
{
    long num = register_get(child, orig_rax);
    printf("%s (%ld)\n", syscalls[num], num);
}

int syscall_wait(pid_t child)
{
    while (1) {
        ptrace(PTRACE_SYSCALL, child, 0, 0);
        int status;
        waitpid(child, &status, 0);
        /* stopped by ptrace */
        if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80))
            return 0;
        if (WIFEXITED(status))
            return 1;
    }
}
