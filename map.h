#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

#include "allocator.h"
#include "comparator.h"
#include "layout.h"

/// @brief Abstract ordered map data type, associating keys to values
typedef struct Map Map;

/// @brief Allocates an empty map
/// @returns The new map, or @c NULL if memory could not be allocated
Map* map_new(Layout key_layout, Layout value_layout, Comparator comparator);

/// @brief Allocates an empty map
/// @returns The new map, or @c NULL if memory could not be allocated
Map* map_new_with(Layout key_layout, Layout value_layout, Comparator comparator, Allocator allocator);

/// @brief Verifies that a map is valid: that is, that no internal invariants are violated
void map_check(const Map* map);

/// @brief Returns the number of key-value pairs stored by a map
size_t map_count(const Map* map);

/// @brief Finds the value associated to a given key, if any
void* map_lookup(const Map* map, const void* key);

/// @brief Associates a key to a value
/// @return @c true on success, @c false if memory could not be allocated
bool map_insert(Map* map, const void* key, const void* value);

/// @brief Removes the value associated to a key, if any
/// @return @c true if an association to the key existed prior to removal, @c false otherwise
bool map_remove(Map* map, const void* key);

/// @brief Copies a map
/// @return The copied map, or @c NULL if memory could not be allocated
Map* map_copy(const Map* map);

/// @brief Copies a map
/// @return The copied map, or @c NULL if memory could not be allocated
Map* map_copy_with(const Map* map, Allocator allocator);

/// @brief Empties a map, clearing all associations from keys to values
void map_clear(Map* map);

/// @brief Clears and deallocates a map
void map_destroy(Map* map);

#endif
