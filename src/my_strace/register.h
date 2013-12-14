#ifndef REGISTER_H
# define REGISTER_H

# include <sys/user.h>

struct user_regs_struct register_get_all(pid_t child);
# define register_get(Child, Name) \
    register_get_offset(Child, (long)(&((struct user *)0)->regs.Name))
long register_get_offset(pid_t child, long offset);

#endif /* !REGISTER_H */
