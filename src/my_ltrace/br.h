#ifndef BR_H
# define BR_H

# include <sys/types.h>
# include <stdint.h>
# include "map.h"

# define get_register(Name, Pid) \
    ptrace(PTRACE_PEEKUSER, Pid, sizeof (long) * Name, 0)

struct breakpoint {
    void *addr;
    const char *name;
    uint64_t old_instr;
};
typedef struct breakpoint breakpoint_s;

void breakpoint_create(map_s *map, uint64_t addr, const char *name, pid_t pid);
void breakpoint_resume(map_s *brkp, pid_t pid);
void breakpoint_print(void *br);

#endif /* !BR_H */
