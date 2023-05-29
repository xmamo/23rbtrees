#include "allocator.h"

#include <stddef.h>
#include <stdlib.h>

static void* heap_allocate(void* data, size_t size) {
  (void)data;
  return malloc(size);
}

static void* heap_reallocate(void* data, void* pointer, size_t size) {
  (void)data;
  return realloc(pointer, size);
}

static void heap_free(void* data, void* pointer) {
  (void)data;
  free(pointer);
}

static const Allocator_methods heap_allocator_methods = {
  .allocate = heap_allocate,
  .reallocate = heap_reallocate,
  .free = heap_free,
};

const Allocator heap_allocator = {
  .data = NULL,
  .methods = &heap_allocator_methods,
};
