#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/fcntl.h>
#include "proc.h"
#include "log.h"

void proc_trace(const char *name, int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    (void) name;
    pid_t child;

    do {
        child = fork();
    } while (child == -1);

    if (child == 0) {
        ptrace(PTRACE_TRACEME);
    }
}

proc_s *proc_open(const char *name, int argc, char *argv[])
{
    //proc_trace(name, argc, argv);
    (void) argc;
    (void) argv;

    proc_s *p = malloc(sizeof (proc_s));
    p->name = name;
    p->fd = open(name, O_RDONLY);
    if (p->fd == -1)
        LOG(ERROR, "Fail to open %s\n", name);
    elf_symbols(p->fd);
    return p;
}
