#ifndef PROC_H
# define PROC_H

# include <stddef.h>
# include <unistd.h>
# include <sys/types.h>
# include "map.h"

struct proc {
    const char *name;
    char **argv;
    pid_t pid;
    int fd;
};
typedef struct proc proc_s;

proc_s *proc_open(char *argv[]);
proc_s *proc_attach(int pid);
void proc_trace(map_s *brkp, proc_s *proc);

#endif /* !PROC_H */
