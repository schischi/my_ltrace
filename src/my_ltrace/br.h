#ifndef BR_H
# define BR_H

struct breakpoint {
    void *addr;
    const char *name;
    unsigned char old_instr;
};
typedef struct breakpoint breakpoint_s;

breakpoint_s *breakpoint_create(unsigned long addr, const char *name);

#endif /* !BR_H */
