#ifndef OPT_H
# define OPT_H

struct opt {
    int rip;
    int follow;
    char **prog_argv;
    int pid;
};
typedef struct opt opt_s;

int opt_parse(int argc, char **argv);

#endif /* !OPT_H */
