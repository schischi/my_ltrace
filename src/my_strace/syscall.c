#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stddef.h>

#include "register.h"
#include "syscall.h"

static const char *syscalls[] = {
#define X(Name, Num) #Name,
#include "syscalls.def"
#undef X
};

typedef enum type { E_PTR, E_INT, E_UINT, E_STR } e_type;

struct arg {
    e_type type;
    const char *name;
};

static const struct sys_def {
    e_type ret;
    const char *name;
    size_t n;
    size_t count;
    struct arg argv[5];
} syscall_def[] = {
#include "syscall_list.inc"
};

static void type_print(pid_t child, long ret, struct arg argv)
{
    if (argv.type == E_INT)
        printf("%s = %ld, ", argv.name, ret);
    else if (argv.type == E_UINT)
        printf("%s = %lu, ", argv.name, (unsigned long)ret);
    else if (argv.type == E_PTR)
        printf("%s = %p, ", argv.name, (void*)ret);
    else if (argv.type == E_STR) {
        printf("%s = \"", argv.name);
        char c;
        int offset = 0;
        while (1) {
            c = ptrace(PTRACE_PEEKDATA, child, ret + offset);
            if (c == 0)
                break;
            printf("%c", c);
            ++offset;
        }
        printf("\", ");
    }
}

static void special_print(pid_t child, unsigned long num, size_t n)
{
    unsigned i = 0;
    printf("%s (", syscalls[num]);
    do {
#define X(Reg)                                                               \
    if (syscall_def[n].count > i)                                            \
        type_print(child, register_get(child, Reg), syscall_def[n].argv[i]); \
    else                                                                     \
        break;                                                               \
    ++i;
#include "calling_conventions.def"
#undef X
    } while (0);
    printf(")");
}

void syscall_print(pid_t child)
{
    unsigned long num = register_get(child, orig_rax);
    for (size_t i = 0; i < sizeof (syscall_def) / sizeof(struct sys_def);
            ++i) {
        if (syscall_def[i].n == num) {
            special_print(child, num, i);
            return;
        }
    }
    printf("%s ()", syscalls[num]);
}

void syscall_print_ret(pid_t child)
{
    long ret = register_get(child, rax);
    printf(" = %ld\n", ret);
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
