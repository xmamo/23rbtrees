#include "layout.h"

Layout layout_init(void) {
  return (Layout){.size = 0, .alignment = 1};
}

size_t layout_add(Layout* layout, Layout member_layout) {
  size_t member_size = member_layout.size;
  size_t member_alignment = member_layout.alignment;

  if (member_alignment == 0) {
    member_alignment = 1;
  }

  size_t member_offset = (layout->size / member_alignment) * member_alignment;

  while (member_offset < layout->size) {
    member_offset += member_alignment;
  }

  layout->size = member_offset + member_size;

  if (member_alignment > layout->alignment) {
    layout->alignment = member_alignment;
  }

  return member_offset;
}

Layout layout_build(const Layout* layout) {
  size_t alignment = layout->alignment;

  if (alignment == 0) {
    alignment = 1;
  }

  size_t size = (layout->size / alignment) * alignment;

  while (size < layout->size) {
    size += alignment;
  }

  return (Layout){.size = size, .alignment = alignment};
}
