#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <stdint.h>
#include "map.h"
#include "log.h"
#include "br.h"

#include <stdio.h>
void dump_memory(pid_t pid, unsigned long from, unsigned long to)
{
    LOG(INFO, "Dump form 0x%lx to 0x%lx", from, to);
    for (unsigned long a = from; a <= to; ++a) {
        unsigned long w = ptrace(PTRACE_PEEKTEXT, pid, a, 0);
        printf("0x%08lx: %02lx\n", a, w & 0xFF);
    }
}

void breakpoint_create(map_s *map, uint64_t addr, const char *name, pid_t pid)
{
    breakpoint_s *bp = malloc(sizeof (breakpoint_s));
    bp->addr = (void *)addr;
    bp->name = name;
    bp->old_instr = 0;
    //if (strcmp("strlen", name))
    //    return;

    /* save current instruction */
    bp->old_instr = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
    LOG(INFO, "Save 0x%lx", bp->old_instr);
    /* replace current instruction with a breakpoint */
    ptrace(PTRACE_POKETEXT, pid, addr,
            (bp->old_instr & (UINT64_MAX - UINT8_MAX)) | 0xCC);
    LOG(INFO, "Instr is 0x%lx", bp->old_instr & (UINT64_MAX - UINT8_MAX) | 0xCC);

    LOG(INFO, "Breakpoint at %p (%s)", bp->addr, bp->name);
    map_add(map, bp, bp->addr);
}

static long get_child_eip(pid_t pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    return regs.rip;
}
#include <assert.h>
#define print_eip() fprintf(stderr, "EIP = 0x%lx\n", get_child_eip(pid))

void breakpoint_resume(map_s *brkp, pid_t pid)
{
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    uint64_t rip = get_child_eip(pid);

    breakpoint_s *b = map_get(brkp, (void*)(uintptr_t)(rip - 1));
    if (b == NULL) {
        LOG(INFO, "Unknow TRAP", NULL);
        return;
    }
    LOG(INFO, "TRAP, RIP=0x%lx, name = \"%s\"", rip, b->name);

    /* restore instruction */
    uint64_t data = ptrace(PTRACE_PEEKTEXT, pid, b->addr, 0);
    ptrace(PTRACE_POKETEXT, pid, b->addr,
            (data & UINT64_MAX - UINT8_MAX) | (b->old_instr & 0xFF));
    /* set EIP, 1 instruction ago */
    regs.rip = (unsigned long)b->addr;
    ptrace(PTRACE_SETREGS, pid, 0, &regs);

    /* exec the instruction */
    int stat;
    ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
    wait(&stat);

    /* set the breakpoint again */
    ptrace(PTRACE_POKETEXT, pid, b->addr,
            (b->old_instr & (UINT64_MAX - UINT8_MAX)) | 0xCC);
    LOG(INFO, "EIP = 0x%lx", get_child_eip(pid));
    ptrace(PTRACE_CONT, pid, 0, 0);
}

void breakpoint_print(void *br)
{
    breakpoint_s *b = br;
    LOG(INFO, "Breakpoints \"%s\" at %p", b->name, b->addr);
}
