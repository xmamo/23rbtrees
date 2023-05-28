#include "comparator.h"

#include <math.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>

static int _char_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  char x = *(char*)_x;
  char y = *(char*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _char_comarator_methods = {
  .compare = _char_compare,
};

const Comparator char_comparator = {
  .data = NULL,
  .methods = &_char_comarator_methods,
};

static int _wchar_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  wchar_t x = *(wchar_t*)_x;
  wchar_t y = *(wchar_t*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _wchar_comarator_methods = {
  .compare = _wchar_compare,
};

const Comparator wchar_comparator = {
  .data = NULL,
  .methods = &_wchar_comarator_methods,
};

static int _schar_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  signed char x = *(signed char*)_x;
  signed char y = *(signed char*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _schar_comarator_methods = {
  .compare = _schar_compare,
};

const Comparator schar_comparator = {
  .data = NULL,
  .methods = &_schar_comarator_methods,
};

static int _short_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  short x = *(short*)_x;
  short y = *(short*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _short_comarator_methods = {
  .compare = _short_compare,
};

const Comparator short_comparator = {
  .data = NULL,
  .methods = &_short_comarator_methods,
};

static int _int_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  int x = *(int*)_x;
  int y = *(int*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _int_comarator_methods = {
  .compare = _int_compare,
};

const Comparator int_comparator = {
  .data = NULL,
  .methods = &_int_comarator_methods,
};

static int _long_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  long x = *(long*)_x;
  long y = *(long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _long_comarator_methods = {
  .compare = _long_compare,
};

const Comparator long_comparator = {
  .data = NULL,
  .methods = &_long_comarator_methods,
};

static int _llong_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  long long x = *(long long*)_x;
  long long y = *(long long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _llong_comarator_methods = {
  .compare = _llong_compare,
};

const Comparator llong_comparator = {
  .data = NULL,
  .methods = &_llong_comarator_methods,
};

static int _uchar_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned char x = *(unsigned char*)_x;
  unsigned char y = *(unsigned char*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _uchar_comarator_methods = {
  .compare = _uchar_compare,
};

const Comparator uchar_comparator = {
  .data = NULL,
  .methods = &_uchar_comarator_methods,
};

static int _ushort_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned short x = *(unsigned short*)_x;
  unsigned short y = *(unsigned short*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _ushort_comarator_methods = {
  .compare = _ushort_compare,
};

const Comparator ushort_comparator = {
  .data = NULL,
  .methods = &_ushort_comarator_methods,
};

static int _uint_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned int x = *(unsigned int*)_x;
  unsigned int y = *(unsigned int*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _uint_comarator_methods = {
  .compare = _uint_compare,
};

const Comparator uint_comparator = {
  .data = NULL,
  .methods = &_uint_comarator_methods,
};

static int _ulong_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned long x = *(unsigned long*)_x;
  unsigned long y = *(unsigned long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _ulong_comarator_methods = {
  .compare = _ulong_compare,
};

const Comparator ulong_comparator = {
  .data = NULL,
  .methods = &_ulong_comarator_methods,
};

static int _ullong_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  unsigned long long x = *(unsigned long long*)_x;
  unsigned long long y = *(unsigned long long*)_y;
  return x < y ? -1 : (x > y ? +1 : 0);
}

static const Comparator_methods _ullong_comarator_methods = {
  .compare = _ullong_compare,
};

const Comparator ullong_comparator = {
  .data = NULL,
  .methods = &_ullong_comarator_methods,
};

static int _float_compare(const void* comparator, const void* _x, const void* _y) {
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

static const Comparator_methods _float_comparator_methods = {
  .compare = _float_compare,
};

const Comparator float_comparator = {
  .data = NULL,
  .methods = &_float_comparator_methods,
};

static int _double_compare(const void* comparator, const void* _x, const void* _y) {
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

static const Comparator_methods _double_comparator_methods = {
  .compare = _double_compare,
};

const Comparator double_comparator = {
  .data = NULL,
  .methods = &_double_comparator_methods,
};

static int _ldouble_compare(const void* comparator, const void* _x, const void* _y) {
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

static const Comparator_methods _ldouble_comparator_methods = {
  .compare = _ldouble_compare,
};

const Comparator ldouble_comparator = {
  .data = NULL,
  .methods = &_ldouble_comparator_methods,
};

static int _string_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  const char* x = _x;
  const char* y = _y;
  return strcmp(x, y);
}

static const Comparator_methods _string_comparator_methods = {
  .compare = _string_compare,
};

const Comparator string_comparator = {
  .data = NULL,
  .methods = &_string_comparator_methods,
};

static int _wstring_compare(const void* comparator, const void* _x, const void* _y) {
  (void)comparator;
  const wchar_t* x = _x;
  const wchar_t* y = _y;
  return wcscmp(x, y);
}

static const Comparator_methods _wstring_comparator_methods = {
  .compare = _wstring_compare,
};

const Comparator wstring_comparator = {
  .data = NULL,
  .methods = &_wstring_comparator_methods,
};
