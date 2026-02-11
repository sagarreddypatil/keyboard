#pragma once

#include <pico/time.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define LIKELY(x) __builtin_expect(static_cast<bool>(x), 1)
#define UNLIKELY(x) __builtin_expect(static_cast<bool>(x), 0)

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))

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

static u64 time_micros()
{
    return to_us_since_boot(get_absolute_time());
}