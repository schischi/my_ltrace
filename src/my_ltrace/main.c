#include <stdlib.h>
#include <stdio.h>
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
    if (opt_parse(argc, argv))
        return 1;
    proc_s *p = proc_open(opts_g.prog_argv);
    brkp = elf_set_breakpoints(p);
    map_sort(brkp);
    map_print(brkp, breakpoint_print);
    proc_trace(brkp, p);
    return 0;
}
