#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/wait.h>
#include "map.h"
#include "log.h"
#include "br.h"
#include "opt.h"

extern opt_s opts_g;

#if 0
static void dump_memory(pid_t pid, unsigned long from, unsigned long to)
{
    LOG(INFO, "Dump form 0x%lx to 0x%lx", from, to);
    for (unsigned long a = from; a <= to; ++a) {
        unsigned long w = ptrace(PTRACE_PEEKTEXT, pid, a, 0);
        printf("0x%08lx: %02lx\n", a, w & 0xFF);
    }
}
#endif

void breakpoint_create(map_s *map, uint64_t addr, const char *name, pid_t pid)
{
    breakpoint_s *bp = malloc(sizeof (breakpoint_s));
    bp->addr = (void *)addr;
    bp->name = name;
    bp->old_instr = 0;
    //if (!strcmp("puts", name))
    //    return;

    /* save current instruction */
    bp->old_instr = ptrace(PTRACE_PEEKTEXT, pid, addr, 0);
    /* replace current instruction with a breakpoint */
    ptrace(PTRACE_POKETEXT, pid, addr,
            (bp->old_instr & (UINT64_MAX - UINT8_MAX)) | 0xCC);
    map_add(map, bp, bp->addr);
}

void breakpoint_resume(map_s *brkp, pid_t pid)
{
    struct user_regs_struct regs;
    uint64_t rip = get_register(RIP, pid);

    breakpoint_s *b = map_get(brkp, (void*)(uintptr_t)(rip - 1));
    if (b == NULL) {
        ptrace(PTRACE_SYSCALL, pid, 0, 0);
        return;
    }
    if (opts_g.rip)
        printf("[0x%lx] ", get_register(RIP, pid));
    printf("%s() =", b->name);

    /* restore instruction */
    uint64_t data = ptrace(PTRACE_PEEKTEXT, pid, b->addr, 0);
    ptrace(PTRACE_POKETEXT, pid, b->addr,
            (data & UINT64_MAX - UINT8_MAX) | (b->old_instr & 0xFF));
    /* set RIP, 1 instruction ago */
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    regs.rip = (unsigned long)b->addr;
    ptrace(PTRACE_SETREGS, pid, 0, &regs);

    /* exec the instruction */
    int stat;
    ptrace(PTRACE_SINGLESTEP, pid, 0, 0);
    wait(&stat);
    printf(" %ld\n", get_register(RAX, pid));

    /* set the breakpoint again */
    ptrace(PTRACE_POKETEXT, pid, b->addr,
            (b->old_instr & (UINT64_MAX - UINT8_MAX)) | 0xCC);
    ptrace(PTRACE_SYSCALL, pid, 0, 0);
}

void breakpoint_print(void *br)
{
    breakpoint_s *b = br;
    printf("Breakpoints \"%s\" at %p\n", b->name, b->addr);
}
