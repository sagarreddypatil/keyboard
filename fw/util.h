#pragma once

#include <stdint.h>
#include <stdio.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int32_t i64;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

template <typename T>
class __Deferrer
{
public:
    __Deferrer(T f) : f_(f)
    {
    }
    ~__Deferrer()
    {
        f_();
    }

private:
    T f_;
};

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#define defer(x)                                                                                   \
    __Deferrer CONCAT(__defferer, __COUNTER__)                                                     \
    {                                                                                              \
        [&] { x; }                                                                                 \
    }
