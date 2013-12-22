#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include "opt.h"

opt_s opts_g;

int opt_parse(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "+ifp:")) != -1) {
        switch (opt) {
            case 'i':
                opts_g.rip = 1;
                break;
            case 'f':
                opts_g.follow = 1;
                break;
            case 'p':
                opts_g.pid = atoi(optarg);;
                break;
            default:
                return 1;
        }
    }
    opts_g.prog_argv = argv + optind;
    return opts_g.pid == 0 &&  optind >= argc;
}
