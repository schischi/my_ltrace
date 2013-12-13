#ifndef PROC_H
# define PROC_H

# include <stddef.h>
# include <unistd.h>

# include "elfd.h"

struct proc {
    const char *name;
    pid_t pid;
    int fd;
};
typedef struct proc proc_s;

proc_s *proc_open(const char *name);

#endif /* !PROC_H */
