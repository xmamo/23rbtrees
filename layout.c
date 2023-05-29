#include "layout.h"

Layout layout_empty(void) {
  return (Layout){.size = 0, .alignment = 1};
}

size_t layout_add(Layout* layout, Layout member_layout) {
  size_t member_size = member_layout.size;
  size_t member_alignment = member_layout.alignment != 0 ? member_layout.alignment : 1;
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

size_t layout_pad(Layout* layout) {
  size_t alignment = layout->alignment != 0 ? layout->alignment : 1;
  size_t new_size = (layout->size / alignment) * alignment;

  while (new_size < layout->size) {
    new_size += alignment;
  }

  return layout->size = new_size;
}
