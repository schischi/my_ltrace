#ifndef MAP_H
# define MAP_H

#include <stddef.h>

struct map_elt {
    const void *key;
    void *value;
};
typedef struct map_elt map_elt_s;

struct map {
    map_elt_s **array;
    int ordered;
    int (*cmp)(const void *e1, const void *e2);
    size_t count;
    size_t size;
};
typedef struct map map_s;

map_s *map_init(size_t size, int(*cmp)(const void *e1, const void *e2));
void map_add(map_s *map, void *value, const void *key);
void map_sort(map_s *map);
void *map_get(map_s *map, const char *key);
void map_print(map_s *map, void(*print)(void *elt));
void map_free(map_s *map);

int cmp_char(const void *p1, const void *p2);
int cmp_addr(const void *p1, const void *p2);

#endif /* !MAP_H */
