#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/fcntl.h>
#include "proc.h"
#include "log.h"
#include "br.h"

#include <sys/reg.h>
#include <sys/user.h>

static pid_t proc_exec(char *argv[])
{
    pid_t pid;

    do {
        pid = fork();
    } while (pid == -1);

    if (pid == 0) {
        ptrace(PTRACE_TRACEME);
        execvp(argv[0], argv);
        LOG(WARN, "Child", NULL);
    }
    waitpid(pid, (int[]){0}, 0);
    return pid;
}

#include <stdio.h>
static void proc_trace(pid_t pid)
{
    int status = 0;
    while (1) {
        waitpid(pid, &status, 0);
        //if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
        //    LOG(INFO, "Trap\n", NULL);
        if (WIFEXITED(status))
            return;
        breakpoint_resume(pid);
    }
}

proc_s *proc_open(char *argv[])
{
    pid_t pid = proc_exec(argv);
    (void)pid;
    proc_s *p = malloc(sizeof (proc_s));
    p->name = argv[0];
    p->fd = open(argv[0], O_RDONLY);
    if (p->fd == -1)
        LOG(ERROR, "Fail to open %s\n", argv[0]);
    elf_symbols(p->fd, pid);
    ptrace(PTRACE_CONT, pid, 0, 0);
    if (1)
        proc_trace(pid);
    return p;
}
