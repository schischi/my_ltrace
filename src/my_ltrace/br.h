#ifndef BR_H
# define BR_H

# include <sys/types.h>
# include <stdint.h>

struct breakpoint {
    void *addr;
    const char *name;
    uint64_t old_instr;
};
typedef struct breakpoint breakpoint_s;

breakpoint_s *breakpoint_create(unsigned long addr, const char *name, pid_t pid);
void breakpoint_resume(pid_t pid);

#endif /* !BR_H */
