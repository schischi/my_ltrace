#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "proc.h"

int main(int argc, char *argv[])
{
#if 0
    map_s *map = map_init(1);
    map_add(map, "foo", "foo");
    map_add(map, "bar", "bar");
    map_add(map, "baz", "baz");
    map_add(map, "egg", "egg");
    map_sort(map);
    map_print(map, NULL);
    map_free(map);
#endif

    if (argc < 2)
        return 1;
    proc_s *p = proc_open(argv[1], argc - 2, &argv[2]);
    (void)p;
    return 0;
}
