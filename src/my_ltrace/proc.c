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
#include <sys/reg.h>
#include <sys/user.h>

#include "proc.h"
#include "log.h"
#include "br.h"

static volatile int quit = 0;

static void sigint_handler(int sig)
{
    (void)sig;
    quit = 1;
}

static pid_t proc_exec(char *argv[])
{
    pid_t pid;

    do {
        pid = fork();
    } while (pid == -1);

    if (pid == 0) {
        ptrace(PTRACE_TRACEME);
        execvp(argv[0], argv);
    }
    waitpid(pid, NULL, 0);
    return pid;
}

void proc_trace(map_s *brkp, proc_s *proc)
{
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigint_handler);
    ptrace(PTRACE_SETOPTIONS, proc->pid, 0, PTRACE_O_TRACESYSGOOD
                                          | PTRACE_O_TRACEFORK
                                          | PTRACE_O_TRACECLONE);
    ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);

    int status = 0;
    while (quit == 0) {
        waitpid(proc->pid, &status, 0);
        /* handle fork, clone, ... */
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
        /* handle syscalls */
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80)) {
            struct user_regs_struct uregs;
            ptrace(PTRACE_GETREGS, proc->pid, 0, &uregs);
            //LOG(INFO, "Syscall %d", uregs.orig_rax);
            ptrace(PTRACE_SYSCALL, proc->pid, 0, 0);
        }
        /* handle breakpoints */
        else if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP)
            breakpoint_resume(brkp, proc->pid);
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
    p->pid = proc_exec(p->argv);
    p->fd = open(argv[0], O_RDONLY);
    if (p->fd == -1)
        LOG(ERROR, "Unable to open %s\n", argv[0]);
    return p;
}

proc_s *proc_attach(int pid)
{
    proc_s *p = malloc(sizeof (proc_s));
    if (kill(pid, 0))
        goto error_1;
    char buf[PATH_MAX];
    snprintf(buf, PATH_MAX, "/proc/%d/exe", pid);
    if (access(buf, F_OK))
        goto error_1;
    p->name = strdup(buf);
    p->argv = NULL;
    p->fd = open(p->name, O_RDONLY);
    p->pid = pid;
    if (p->fd == -1)
        goto error_1;
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL))
        goto error_2;
    waitpid(pid, NULL, 0);
    return p;

error_2:
    close(p->fd);
error_1:
    free(p);
    LOG(ERROR, "Unable to attach to %d", pid);
    return NULL;
}

void proc_close(proc_s *proc, int pid)
{
    if (pid != 0) {
        waitpid(proc->pid, NULL, 0);
        if (ptrace(PTRACE_DETACH, pid, NULL, NULL)) {
            LOG(ERROR, "Unable to detach from %d", pid);
        }
        printf("Detached from %d\n", pid);
    }
    close(proc->fd);
    free(proc);
}
