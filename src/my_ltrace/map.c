#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "map.h"
#include "log.h"

map_s *map_init(size_t size, int(*cmp)(const void *e1, const void *e2))
{
    map_s *map = malloc(sizeof (map_s));
    map->size = size;
    map->ordered = 0;
    map->count = 0;
    map->array = malloc(sizeof (map_elt_s) * size);
    map->cmp = cmp;
    return map;
}

void map_add(map_s *map, void *value, const void *key)
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

int cmp_char(const void *p1, const void *p2)
{
    const char* k1 = (*(map_elt_s *const *)p1)->key;
    const char* k2 = (*(map_elt_s *const *)p2)->key;
    return strcmp(k1, k2);
}

int cmp_addr(const void *p1, const void *p2)
{
    const void *a1 = (*(map_elt_s *const *)p1)->key;
    const void *a2 = (*(map_elt_s *const *)p2)->key;
    //fprintf(stderr, "About to cmp %p and %p\n", a1, a2);
    return (uintptr_t)a1 - (uintptr_t)a2;
}

int cmp_addr_with(const void *p1, const void *p2)
{
    //fprintf(stderr, "Cmp %p and %p\n", p1, p2);
    return (uintptr_t)p1 - (uintptr_t)p2;
}

void map_sort(map_s *map)
{
    qsort(map->array, map->count, sizeof (const char *), map->cmp);
    map->ordered = 1;
}

void *map_get(map_s *map, const char *key)
{
    if (map->ordered == 0)
        map_sort(map);
    int min = 0;
    int max = map->count;
    while (min <= max) {
        int mid = (min + max) / 2;
        int ret = cmp_addr_with(map->array[mid]->key, key);
        if (ret == 0)
            return map->array[mid]->value;
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
            print(map->array[i]->value);
    }
}

void map_free(map_s *map)
{
    for (size_t i = 0; i < map->count; ++i)
        free(map->array[i]);
    free(map->array);
    free(map);
}
