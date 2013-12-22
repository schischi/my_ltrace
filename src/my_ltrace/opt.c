#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "opt.h"

opt_s opts_g;

int opt_parse(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "+if")) != -1) {
        switch (opt) {
            case 'i':
                opts_g.rip = 1;
                break;
            case 'f':
                opts_g.follow = 1;
                break;
            default:
                return 1;
        }
    }
    opts_g.prog_argv = argv + optind;
    return optind >= argc;
}
