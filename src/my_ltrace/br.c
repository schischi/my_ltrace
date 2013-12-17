#include <stdlib.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include "log.h"
#include "br.h"

breakpoint_s *breakpoint_create(unsigned long addr, const char *name)
{
    breakpoint_s *bp = malloc(sizeof (breakpoint_s));
    bp->addr = (void *)(uintptr_t)addr;
    bp->name = name;
    bp->old_instr = 0;
    LOG(INFO, "Breakpoint at %p (%s)", bp->addr, bp->name);

    unsigned char instr = 0;
    //ptrace(PTRACE_PEEKTEXT, pid, &instr, 0);
    LOG(INFO, "Instr is 0x%x\n", instr);

    return bp;
}
