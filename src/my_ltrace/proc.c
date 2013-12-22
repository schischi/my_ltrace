#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <limits.h>
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
    LOG(INFO, "Trace %d", proc->pid);
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
            ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
        }
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80)) {
            struct user_regs_struct uregs;
            ptrace(PTRACE_GETREGS, proc->pid, 0, &uregs);
            //LOG(INFO, "Syscall %d", uregs.orig_rax);
            ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
        }
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
            breakpoint_resume(brkp, proc->pid, status);
        }
        else if (WIFEXITED(status)) {
            printf("Exited\n");
            return;
        }
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

proc_s *proc_attach(int pid)
{
    proc_s *p = malloc(sizeof (proc_s));
    if (kill(pid, 0)) {
        LOG(ERROR, "Fail to attach to %d\n", pid);
        exit(1);
    }
    char buf[PATH_MAX];
    snprintf(buf, 128, "/proc/%d/exe", pid);
    if (access(buf, F_OK)) {
        LOG(ERROR, "Fail to retrieve the exe of %d\n", pid);
        exit(1);
    }
    p->name = strdup(buf);
    p->argv = NULL;
    p->fd = open(p->name, O_RDONLY);
    p->pid = pid;
    if (p->fd == -1) {
        LOG(ERROR, "Fail to open %s\n", p->name);
        exit(1);
    }
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL)) {
        LOG(ERROR, "Failed to attach to %d", pid);
        exit(1);
    }
    //kill(pid, SIGSTOP);
    waitpid(pid, (int[]){0}, 0);
    return p;
}
