#include <stdlib.h>
#include <stdio.h>
#include "map.h"

int main()
{
    map_s *map = map_init(1);
    map_add(map, "foo", "foo");
    map_add(map, "bar", "bar");
    map_add(map, "baz", "baz");
    map_add(map, "egg", "egg");
    map_sort(map);
    map_print(map, NULL);
    map_free(map);
    return 0;
}
