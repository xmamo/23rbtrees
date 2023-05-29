#include "allocator.h"

#include <stddef.h>
#include <stdlib.h>

static void* _heap_allocate(void* data, size_t size) {
  (void)data;
  return malloc(size);
}

static void* _heap_reallocate(void* data, void* pointer, size_t size) {
  (void)data;
  return realloc(pointer, size);
}

static void _heap_free(void* data, void* pointer) {
  (void)data;
  free(pointer);
}

static const Allocator_methods _heap_allocator_methods = {
  .allocate = _heap_allocate,
  .reallocate = _heap_reallocate,
  .free = _heap_free,
};

const Allocator heap_allocator = {
  .data = NULL,
  .methods = &_heap_allocator_methods,
};
