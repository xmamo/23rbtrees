#ifndef COMPARATOR_H
#define COMPARATOR_H

typedef struct Comparator_methods {
  int (*compare)(const void* data, const void* x, const void* y);
} Comparator_methods;

typedef struct Comparator {
  void* data;
  const Comparator_methods* methods;
} Comparator;

extern const Comparator char_comparator;
extern const Comparator wchar_comparator;

extern const Comparator schar_comparator;
extern const Comparator short_comparator;
extern const Comparator int_comparator;
extern const Comparator long_comparator;
extern const Comparator llong_comparator;

extern const Comparator uchar_comparator;
extern const Comparator ushort_comparator;
extern const Comparator uint_comparator;
extern const Comparator ulong_comparator;
extern const Comparator ullong_comparator;

extern const Comparator float_comparator;
extern const Comparator double_comparator;
extern const Comparator ldouble_comparator;

extern const Comparator string_comparator;
extern const Comparator wstring_comparator;

static inline int comparator_compare(Comparator comparator, const void* x, const void* y) {
  return comparator.methods->compare(comparator.data, x, y);
}

#endif
