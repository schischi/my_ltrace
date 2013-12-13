#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "map.h"

map_s *map_init(size_t size)
{
    map_s *map = malloc(sizeof (map_s));
    map->size = size;
    map->ordered = 0;
    map->count = 0;
    map->array = malloc(sizeof (map_elt_s) * size);
    return map;
}

void map_add(map_s *map, void *value, const char *key)
{
    map_elt_s *elt = malloc(sizeof (map_elt_s));
    elt->key = key;
    elt->value = value;
    if (map->count == map->size) {
        map->size *= 2;
        map->array = realloc(map->array, map->size * sizeof (map_elt_s));
    }
    map->array[map->count] = elt;
    map->count += 1;
    map->ordered = 0;
}

static int cmp_key(const void *p1, const void *p2)
{
    const char* k1 = (*(map_elt_s *const *)p1)->key;
    const char* k2 = (*(map_elt_s *const *)p2)->key;
    return strcmp(k1, k2);
}

void map_sort(map_s *map)
{
    qsort(map->array, map->count, sizeof (const char *), cmp_key);
    map->ordered = 1;
}

void *map_get(map_s *map, const char *key)
{
    if (map->ordered == 1)
        map_sort(map);
    size_t min = 0;
    size_t max = map->size;
    while (min < max) {
        size_t mid = (min + max) / 2;
        int ret = strcmp(map->array[mid]->key, key);
        if (ret == 0)
            return map->array[mid];
        if (ret > 0)
            max = mid - 1;
        else
            min = mid + 1;
    }

    return NULL;
}

void map_print(map_s *map, void(*print)(void *elt))
{
    for (size_t i = 0; i < map->count; ++i) {
        if (print == NULL)
            printf("%s = %p\n", map->array[i]->key, map->array[i]->value);
        else
            print(map->array[i]);
    }
}

void map_free(map_s *map)
{
    for (size_t i = 0; i < map->count; ++i)
        free(map->array[i]);
    free(map->array);
    free(map);
}
