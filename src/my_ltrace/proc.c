#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include "proc.h"
#include "log.h"

proc_s *proc_open(const char *name)
{
    proc_s *p = malloc(sizeof (proc_s));
    p->name = name;
    p->fd = open(name, O_RDONLY);
    if (p->fd == -1)
        LOG(ERROR, "Fail to open %s\n", name);
    elf_symbols(p->fd);
    return p;
}
