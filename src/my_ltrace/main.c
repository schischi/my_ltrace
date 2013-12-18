#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "proc.h"
#include "elfd.h"
#include "br.h"

int main(int argc, char *argv[])
{
    map_s *brkp;
    if (argc < 2)
        return 1;
    proc_s *p = proc_open(&argv[1]);
    brkp = elf_set_breakpoints(p);
    map_sort(brkp);
    map_print(brkp, breakpoint_print);
    proc_trace(brkp, p);
    return 0;
}
