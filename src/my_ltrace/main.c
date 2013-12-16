#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "proc.h"

int main()
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

    proc_s *p = proc_open("a.out");
    (void)p;
    return 0;
}
