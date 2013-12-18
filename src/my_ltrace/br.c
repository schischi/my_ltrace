#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
# include <stdint.h>
#include "log.h"
#include "br.h"

uint64_t in_g = 0;
void *a_g = NULL;

#include <stdio.h>
void dump_memory(pid_t pid, unsigned long from, unsigned long to)
{
    LOG(INFO, "Dump form 0x%lx to 0x%lx", from, to);
    for (unsigned long a = from; a <= to; ++a) {
        unsigned long w = ptrace(PTRACE_PEEKTEXT, pid, a, 0);
        printf("0x%08lx: %02lx\n", a, w & 0xFF);
    }
}

breakpoint_s *breakpoint_create(unsigned long addr, const char *name, pid_t pid)
{
    breakpoint_s *bp = malloc(sizeof (breakpoint_s));
    bp->addr = (void *)(uintptr_t)addr;
    bp->name = name;
    bp->old_instr = 0;
    if (strcmp("strlen", name))
        return NULL;
    LOG(INFO, "Breakpoint at %p (%s)", bp->addr, bp->name);
    a_g = bp->addr;

    /* save current instruction */
    bp->old_instr = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
    LOG(INFO, "Save 0x%lx", bp->old_instr);
    LOG(INFO, "Instr is 0x%lx", bp->old_instr);
    in_g = bp->old_instr;
    /* replace current instruction with a breakpoint */
    ptrace(PTRACE_POKETEXT, pid, addr, (bp->old_instr & 0xFFFFFFFFFFFFFF00) | 0xCC);

    return bp;
}

static long get_child_eip(pid_t pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    return regs.rip;
}
#include <assert.h>
#define print_eip() fprintf(stderr, "EIP = 0x%lx\n", get_child_eip(pid))

void breakpoint_resume(pid_t pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    print_eip();
    LOG(INFO, "EIP = 0x%lx, br = 0x%lx", get_child_eip(pid), a_g);

    /* restore instruction */
    uint64_t data = ptrace(PTRACE_PEEKTEXT, pid, a_g, 0);
    //if ((data & 0xFF) != 0xCC) {
    //    ptrace(PTRACE_CONT, pid, 0, 0);
    //    return;
    //}
    //assert((data & 0xFF) == 0xCC);
    ptrace(PTRACE_POKETEXT, pid, a_g, (data & 0xFFFFFFFFFFFFFF00) | (in_g & 0xFF));
    /* set EIP, 1 instruction ago*/
    regs.rip = (unsigned long)a_g;
    ptrace(PTRACE_SETREGS, pid, 0, &regs);
    LOG(INFO, "EIP = 0x%lx, br = 0x%lx", get_child_eip(pid), a_g);

    //for (int i = 0; i < 10; ++i) {
    //    ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
    //    int stat;
    //    wait(&stat);
    //    LOG(INFO, "EIP = 0x%lx", get_child_eip(pid));
    //}
    ptrace(PTRACE_CONT, pid, 0, 0);
}
