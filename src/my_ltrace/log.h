#ifndef LOG_H
# define LOG_H

# define COLOR_BLUE    "\x1b[34m"
# define COLOR_RED     "\x1b[31m"
# define COLOR_YELLOW  "\x1b[33m"
# define COLOR_RESET   "\x1b[0m"

enum log_level
{
    INFO,
    WARN,
    ERROR
};
typedef enum log_level e_log_level;

struct debug_info
{
    char *file;
    int line;
};
typedef struct debug_info s_debug_info;

void my_log(e_log_level level, s_debug_info debug, char *Fmt, ...);

# define LOG(Level, Fmt, ...)                     \
    do {                                          \
        s_debug_info debug =                      \
        {                                         \
            .file = __FILE__,                     \
            .line = __LINE__                      \
        };                                        \
        my_log(Level, debug, Fmt, __VA_ARGS__);   \
    } while (0)

#endif /* !LOG_H */
