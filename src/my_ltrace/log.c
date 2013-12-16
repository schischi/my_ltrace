#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include "log.h"

void my_log(e_log_level level, s_debug_info debug, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if (level == INFO)
    {
        fprintf(stdout, "%-17s", COLOR_BLUE"[INFO]"COLOR_RESET);
        vfprintf(stdout, fmt, ap);
        fprintf(stdout, "\n");
    }
    else if (level == WARN)
    {
        fprintf(stderr, "%-17s", COLOR_YELLOW"[WARN]"COLOR_RESET);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, " at (%s:%d)\n", debug.file, debug.line);
    }
    else if (level == ERROR)
    {
        int err = errno;
        fprintf(stderr, "%-17s", COLOR_RED"[ERROR]"COLOR_RESET);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, " at (%s:%d)\n", debug.file, debug.line);
        if (err)
            fprintf(stderr, "\t%s\n", strerror(errno));
    }
    va_end(ap);
}
