#ifndef LAYOUT_H
#define LAYOUT_H

#include <stddef.h>

typedef struct Layout {
  size_t size;
  size_t alignment;
} Layout;

Layout layout_init(void);

size_t layout_add(Layout* layout, Layout member_layout);

Layout layout_build(const Layout* layout);

#endif
