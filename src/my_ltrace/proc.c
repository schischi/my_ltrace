#define _POSIX_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/fcntl.h>
#include <signal.h>
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

void proc_trace(map_s *brkp, proc_s *proc)
{
    ptrace(PTRACE_SETOPTIONS, proc->pid, 0, PTRACE_O_TRACESYSGOOD
           | PTRACE_O_TRACEFORK |  PTRACE_O_TRACECLONE);
    ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);

    int status = 0;
    while (1) {
        waitpid(proc->pid, &status, 0);
        if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_FORK << 8))
            || status >> 8 == (SIGTRAP | (PTRACE_EVENT_CLONE << 8))) {
            unsigned long data;
            ptrace(PTRACE_GETEVENTMSG, proc->pid, NULL, &data);
            LOG(WARN, "Fork to %d", data);
            int s;
            while (1) {
                waitpid(data, &s, 0);
                if (WIFEXITED(s))
                    break;
                ptrace(PTRACE_SYSCALL, data, 0, 0);
            }
            LOG(WARN, "Fork exit", NULL);
            //ptrace(PTRACE_SYSCALL, data, 0, 0);
            //kill(data, SIGSTOP);
            //ptrace(PTRACE_DETACH, data, 0, 0);
            ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
        }
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80)) {
            struct user_regs_struct uregs;
            ptrace(PTRACE_GETREGS, proc->pid, 0, &uregs);
            //LOG(INFO, "Syscall %d", uregs.orig_rax);
            //ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
            //wait(&status);
            ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
        }
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
            breakpoint_resume(brkp, proc->pid, status);
        }
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
