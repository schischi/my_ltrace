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
void proc_trace(map_s *brkp, proc_s *proc)
{
    ptrace(PTRACE_CONT, proc->pid, 0, 0);
    int status = 0;
    while (1) {
        waitpid(proc->pid, &status, 0);
        if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
            breakpoint_resume(brkp, proc->pid);
        else if (WIFEXITED(status))
            return;
    }
}

proc_s *proc_open(char *argv[])
{
    proc_s *p = malloc(sizeof (proc_s));
    p->name = argv[0];
    p->argv = argv;
    p->fd = open(argv[0], O_RDONLY);
    p->pid = proc_exec(p->argv);
    if (p->fd == -1)
        LOG(ERROR, "Fail to open %s\n", argv[0]);
    return p;
}
