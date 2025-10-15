#include "util.h"
#include "config.h"

#include <hardware/gpio.h>
#include <hardware/timer.h>
#include <pico/platform/common.h>
#include <pico/stdio_usb.h>
#include <pico/stdlib.h>
#include <class/hid/hid.h>

void matrix_init()
{
    for (u32 i = 0; i < kRows; ++i)
    {
        const u32 pin = kRowPins[i];
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_down(pin);
    }
    for (u32 i = 0; i < kCols; ++i)
    {
        const u32 pin = kColPins[i];
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, false);
    }
}

void matrix_scan()
{
    for (u32 ci = 0; ci < kCols; ++ci)
    {
        const u32 col = kColPins[ci];
        gpio_put(col, true);
        sleep_us(1); // tested 10us
        {
            for (u32 ri = 0; ri < kRows; ++ri)
            {
                const u32 row = kRowPins[ri];
                if (gpio_get(row))
                {
                    printf("pressed row=%d col=%d\n", ri, ci);
                    fflush(stdout);
                }
            }
        }
        gpio_put(col, false);
    }
}

int main()
{
    stdio_init_all();
    while (!stdio_usb_connected())
    {
        tight_loop_contents();
    }
    stdio_flush();
    for (int i = 0; i < 10; i++)
    {
        printf("\n");
    }
    printf("kbfw begin.\n");

    matrix_init();

    const u32 ip = 0;
    u32 i = 0;
    while (true)
    {
        printf("iter %d\n", i++);
        matrix_scan();
        sleep_ms(100);
    }
}