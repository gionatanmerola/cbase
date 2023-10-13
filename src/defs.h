#ifndef CBASE_DEFS
#define CBASE_DEFS

#define function
#define intern static
#define global static

#include <stdint.h>
#include <stddef.h>

typedef unsigned char uchar;
typedef signed char schar;
typedef unsigned short ushort;
typedef signed short sshort;

typedef unsigned int uint;
typedef unsigned long ul;
typedef unsigned long ulong;
typedef unsigned long long ull;
typedef int long il;
typedef int long ilong;
typedef int long long ill;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef int bool;

typedef float f32;
typedef double f64;

#define MIN(x, y) (((x)<(y))?(x):(y))
#define MAX(x, y) (((x)>(y))?(x):(y))
#define CLAMP(a, x, b) (((x)<(a))?(a):(((x)>(b))?(b):(x)))
#define CLAMP_TOP(a, b) MIN(a,b)
#define CLAMP_BOTTOM(a, b) MAX(a,b)

#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))

#define MACRO_STMT(s) do{s;}while(0)

#define ASSERT_BREAK (*((int*)0)=0)
#define ASSERT(cond) MACRO_STMT(if(!(cond)){ASSERT_BREAK;})

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

#define CONCAT_TOKENS_(a, b) a##b
#define CONCAT_TOKENS(a, b) CONCAT_TOKENS_(a, b)

intern function
void
mem_zero(void *block, size_t size)
{
    size_t i;
    u8 *ptr;

    ptr = (u8 *)block;
    for(i = 0; i < size; ++i)
    {
        *ptr++ = 0;
    }
}

#endif
