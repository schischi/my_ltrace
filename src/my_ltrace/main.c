#include <stdlib.h>
#include <stdio.h>
#include <sys/ptrace.h>
#include "map.h"
#include "proc.h"
#include "elfd.h"
#include "br.h"
#include "opt.h"
#include "log.h"

extern opt_s opts_g;

int main(int argc, char *argv[])
{
    map_s *brkp;
    proc_s *p;
    if (opt_parse(argc, argv))
        return 1;
    if (opts_g.pid == 0)
        p = proc_open(opts_g.prog_argv);
    else
        p = proc_attach(opts_g.pid);
    brkp = elf_set_breakpoints(p);
    map_sort(brkp);
    map_print(brkp, breakpoint_print);
    proc_trace(brkp, p);
    proc_close(p, opts_g.pid);
    return 0;
}
