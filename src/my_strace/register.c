#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stddef.h>
#include "register.h"

struct user_regs_struct register_get_all(pid_t child)
{
    struct user_regs_struct uregs;
    ptrace(PTRACE_GETREGS, child, 0, &uregs);
    return uregs;
}

long register_get_offset(pid_t child, long offset)
{
    return ptrace(PTRACE_PEEKUSER, child, offset);
}
