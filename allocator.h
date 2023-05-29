#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

typedef struct Allocator_methods {
  void* (*allocate)(void* data, size_t size);
  void* (*reallocate)(void* data, void* pointer, size_t size);
  void (*free)(void* data, void* pointer);
} Allocator_methods;

typedef struct Allocator {
  void* data;
  const Allocator_methods* methods;
} Allocator;

static inline void* allocator_allocate(Allocator allocator, size_t size) {
  return allocator.methods->allocate(allocator.data, size);
}

static inline void* allocator_reallocate(Allocator allocator, void* pointer, size_t size) {
  return allocator.methods->reallocate(allocator.data, pointer, size);
}

static inline void allocator_free(Allocator allocator, void* pointer) {
  allocator.methods->free(allocator.data, pointer);
}

extern const Allocator heap_allocator;

#endif
