#include "config.h"
#include "util.h"

#include <bsp/board_api.h>
#include <hardware/gpio.h>
#include <pico/platform/common.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <tusb.h>

u8 keycodes[6]{};
u8 prev_keycodes[6]{};
u8 modifiers = 0;
u8 prev_modifiers = 0;

void matrix_init()
{
    for (u32 i = 0; i < kRows; ++i)
    {
        const u32 pin = kRowPins[i];
        gpio_init(pin);
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
    u32 i = 0;
    modifiers = 0;

    for (u32 ci = 0; ci < kCols; ++ci)
    {
        const u32 col = kColPins[ci];
        gpio_put(col, true);
        sleep_us(1);
        {
            for (u32 ri = 0; ri < kRows; ++ri)
            {
                const u32 row = kRowPins[ri];
                if (gpio_get(row))
                {
                    const u8 kc = kKeymap[ri][ci];
                    printf("key row=%d col=%d name=%s\n", ri, ci, kKeymapStr[ri][ci]);

                    if (kc >= HID_KEY_CONTROL_LEFT && kc <= HID_KEY_GUI_RIGHT)
                    {
                        modifiers |= (1 << (kc - HID_KEY_CONTROL_LEFT));
                    }
                    else if (i < ARRAY_SIZE(keycodes))
                    {
                        keycodes[i++] = kc;
                    }
                }
            }
        }
        gpio_put(col, false);
    }
    for (; i < ARRAY_SIZE(keycodes); ++i)
    {
        keycodes[i] = 0;
    }
}

bool keys_changed()
{
    if (modifiers != prev_modifiers)
        return true;

    for (u32 i = 0; i < ARRAY_SIZE(keycodes); ++i)
    {
        if (keycodes[i] != prev_keycodes[i])
            return true;
    }
    return false;
}

void send_hid_report()
{
    if (tud_suspended() && keys_changed())
    {
        tud_remote_wakeup();
        return;
    }
    if (tud_hid_ready() && keys_changed())
    {
        tud_hid_keyboard_report(0x01, modifiers, keycodes);
        prev_modifiers = modifiers;
        for (u32 i = 0; i < ARRAY_SIZE(keycodes); ++i)
        {
            prev_keycodes[i] = keycodes[i];
        }
    }
}

int main()
{
    board_init();
    tusb_init();
    if (board_init_after_tusb)
    {
        board_init_after_tusb();
    }
    stdio_init_all();

    matrix_init();

    for (u32 i = 0; i < ARRAY_SIZE(prev_keycodes); ++i)
    {
        prev_keycodes[i] = 0xFF;
    }
    prev_modifiers = 0xFF;

    while (true)
    {
        tud_task();

        matrix_scan();
        send_hid_report();

        sleep_ms(10);
    }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                               uint8_t *buffer, uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type,
                           uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)report;
    (void)len;
}