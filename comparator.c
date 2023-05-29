#include "comparator.h"

#include <math.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>

static int char_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  char x = *(char*)_x;
  char y = *(char*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods char_comparator_methods = {
  .compare = char_compare,
};

const Comparator char_comparator = {
  .data = NULL,
  .methods = &char_comparator_methods,
};

static int wchar_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  wchar_t x = *(wchar_t*)_x;
  wchar_t y = *(wchar_t*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods wchar_comparator_methods = {
  .compare = wchar_compare,
};

const Comparator wchar_comparator = {
  .data = NULL,
  .methods = &wchar_comparator_methods,
};

static int schar_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  signed char x = *(signed char*)_x;
  signed char y = *(signed char*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods schar_comparator_methods = {
  .compare = schar_compare,
};

const Comparator schar_comparator = {
  .data = NULL,
  .methods = &schar_comparator_methods,
};

static int short_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  short x = *(short*)_x;
  short y = *(short*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods short_comparator_methods = {
  .compare = short_compare,
};

const Comparator short_comparator = {
  .data = NULL,
  .methods = &short_comparator_methods,
};

static int int_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  int x = *(int*)_x;
  int y = *(int*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods int_comparator_methods = {
  .compare = int_compare,
};

const Comparator int_comparator = {
  .data = NULL,
  .methods = &int_comparator_methods,
};

static int long_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  long x = *(long*)_x;
  long y = *(long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods long_comparator_methods = {
  .compare = long_compare,
};

const Comparator long_comparator = {
  .data = NULL,
  .methods = &long_comparator_methods,
};

static int llong_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  long long x = *(long long*)_x;
  long long y = *(long long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods llong_comparator_methods = {
  .compare = llong_compare,
};

const Comparator llong_comparator = {
  .data = NULL,
  .methods = &llong_comparator_methods,
};

static int uchar_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned char x = *(unsigned char*)_x;
  unsigned char y = *(unsigned char*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods uchar_comparator_methods = {
  .compare = uchar_compare,
};

const Comparator uchar_comparator = {
  .data = NULL,
  .methods = &uchar_comparator_methods,
};

static int ushort_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned short x = *(unsigned short*)_x;
  unsigned short y = *(unsigned short*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods ushort_comparator_methods = {
  .compare = ushort_compare,
};

const Comparator ushort_comparator = {
  .data = NULL,
  .methods = &ushort_comparator_methods,
};

static int uint_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned int x = *(unsigned int*)_x;
  unsigned int y = *(unsigned int*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods uint_comparator_methods = {
  .compare = uint_compare,
};

const Comparator uint_comparator = {
  .data = NULL,
  .methods = &uint_comparator_methods,
};

static int ulong_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned long x = *(unsigned long*)_x;
  unsigned long y = *(unsigned long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods ulong_comparator_methods = {
  .compare = ulong_compare,
};

const Comparator ulong_comparator = {
  .data = NULL,
  .methods = &ulong_comparator_methods,
};

static int ullong_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned long long x = *(unsigned long long*)_x;
  unsigned long long y = *(unsigned long long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods ullong_comparator_methods = {
  .compare = ullong_compare,
};

const Comparator ullong_comparator = {
  .data = NULL,
  .methods = &ullong_comparator_methods,
};

static int float_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  float x = *(float*)_x;
  float y = *(float*)_y;

  if (isnan(x)) {
    int sx = signbit(x) ? -1 : +1;

    if (isnan(y)) {
      int sy = signbit(y) ? -1 : +1;
      return sx < sy ? -1 : (sx > sy ? +1 : 0);
    }

    return sx;
  }

  if (isnan(y)) {
    int sy = signbit(y) ? -1 : +1;
    return -sy;
  }

  if (x == 0 && y == 0) {
    int sx = signbit(x) ? -1 : +1;
    int sy = signbit(y) ? -1 : +1;
    return sx < sy ? -1 : (sx > sy ? +1 : 0);
  }

  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods float_comparator_methods = {
  .compare = float_compare,
};

const Comparator float_comparator = {
  .data = NULL,
  .methods = &float_comparator_methods,
};

static int double_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  double x = *(double*)_x;
  double y = *(double*)_y;

  if (isnan(x)) {
    int sx = signbit(x) ? -1 : +1;

    if (isnan(y)) {
      int sy = signbit(y) ? -1 : +1;
      return sx < sy ? -1 : (sx > sy ? +1 : 0);
    }

    return sx;
  }

  if (isnan(y)) {
    int sy = signbit(y) ? -1 : +1;
    return -sy;
  }

  if (x == 0 && y == 0) {
    int sx = signbit(x) ? -1 : +1;
    int sy = signbit(y) ? -1 : +1;
    return sx < sy ? -1 : (sx > sy ? +1 : 0);
  }

  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods double_comparator_methods = {
  .compare = double_compare,
};

const Comparator double_comparator = {
  .data = NULL,
  .methods = &double_comparator_methods,
};

static int ldouble_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  long double x = *(long double*)_x;
  long double y = *(long double*)_y;

  if (isnan(x)) {
    int sx = signbit(x) ? -1 : +1;

    if (isnan(y)) {
      int sy = signbit(y) ? -1 : +1;
      return sx < sy ? -1 : (sx > sy ? +1 : 0);
    }

    return sx;
  }

  if (isnan(y)) {
    int sy = signbit(y) ? -1 : +1;
    return -sy;
  }

  if (x == 0 && y == 0) {
    int sx = signbit(x) ? -1 : +1;
    int sy = signbit(y) ? -1 : +1;
    return sx < sy ? -1 : (sx > sy ? +1 : 0);
  }

  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods ldouble_comparator_methods = {
  .compare = ldouble_compare,
};

const Comparator ldouble_comparator = {
  .data = NULL,
  .methods = &ldouble_comparator_methods,
};

static int string_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  const char* x = _x;
  const char* y = _y;
  return strcmp(x, y);
}

static const Comparator_methods string_comparator_methods = {
  .compare = string_compare,
};

const Comparator string_comparator = {
  .data = NULL,
  .methods = &string_comparator_methods,
};

static int wstring_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  const wchar_t* x = _x;
  const wchar_t* y = _y;
  return wcscmp(x, y);
}

static const Comparator_methods wstring_comparator_methods = {
  .compare = wstring_compare,
};

const Comparator wstring_comparator = {
  .data = NULL,
  .methods = &wstring_comparator_methods,
};
