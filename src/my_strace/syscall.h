#ifndef SYSCALL_H
# define SYSCALL_H

void syscall_print(pid_t child);
int syscall_wait(pid_t child);

#endif /* !SYSCALL_H */
