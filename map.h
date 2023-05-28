#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "comparator.h"
#include "layout.h"

typedef struct Map Map;

Map* map_new(Layout key_layout, Layout value_layout, Comparator comparator);

size_t map_count(const Map* map);

void* map_lookup(const Map* map, const void* key);

bool map_insert(Map* map, const void* key, const void* value);

bool map_remove(Map* map, const void* key);

Map* map_copy(const Map* map);

void map_clear(Map* map);

void map_check(const Map* map);

#endif
