#ifndef LAYOUT_H
#define LAYOUT_H

#include <stddef.h>

/// @brief Run-time memory layout representation
typedef struct Layout {
  /// @brief The size of the object
  size_t size;

  /// @brief The alignment of the object
  size_t alignment;
} Layout;

/// @brief Returns the layout for empty objects of size @c 0
Layout layout_empty(void);

/// @brief Updates a layout to reflect the presence of one additional member
/// @return The offset of the added member
size_t layout_add(Layout* layout, Layout member_layout);

/// @brief Rounds up the size of a layout to the nearest multiple of its alignment
/// @return The new size of the layout
size_t layout_pad(Layout* layout);

#endif
