#ifndef MAP_H
# define MAP_H

#include <stddef.h>

struct map_elt {
    const char *key;
    void *value;
};
typedef struct map_elt map_elt_s;

struct map {
    map_elt_s **array;
    int ordered;
    size_t count;
    size_t size;
};
typedef struct map map_s;

map_s *map_init(size_t size);
void map_add(map_s *map, void *value, const char *key);
void map_sort(map_s *map);
void *map_get(map_s *map, const char *key);
void map_print(map_s *map, void(*print)(void *elt));
void map_free(map_s *map);

#endif /* !MAP_H */
