#define length(array) (sizeof(array) / sizeof((array)[0]))

typedef unsigned int  B32;
typedef unsigned char U8;
typedef unsigned int  U32;
typedef unsigned long U64;
typedef int           I32;
typedef long          I64;

template <typename T>
static T min(T a, T b) {
  return a < b ? a : b;
}

template <typename T>
static T max(T a, T b) {
  return a > b ? a : b;
}
