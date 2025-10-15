#include "config.h"
#include "util.h"

#include <hardware/gpio.h>
#include <pico/platform/common.h>
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

                    // Check if it's a modifier key
                    if (kc >= HID_KEY_CONTROL_LEFT && kc <= HID_KEY_GUI_RIGHT)
                    {
                        // Set modifier bit
                        modifiers |= (1 << (kc - HID_KEY_CONTROL_LEFT));
                    }
                    else if (i < ARRAY_SIZE(keycodes))
                    {
                        // Regular key
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
    // Only send if USB is mounted, HID is ready, AND keys have changed
    if (tud_mounted() && tud_hid_ready() && keys_changed())
    {
        tud_hid_keyboard_report(0x01, modifiers, keycodes);

        // Copy current to previous
        prev_modifiers = modifiers;
        for (u32 i = 0; i < ARRAY_SIZE(keycodes); ++i)
        {
            prev_keycodes[i] = keycodes[i];
        }
    }
}

int main()
{
    tusb_init();
    matrix_init();

    // Initialize previous state
    for (u32 i = 0; i < ARRAY_SIZE(prev_keycodes); ++i)
    {
        prev_keycodes[i] = 0xFF; // Force first report
    }
    prev_modifiers = 0xFF;

    while (true)
    {
        tud_task(); // TinyUSB device task

        matrix_scan();
        send_hid_report();

        sleep_ms(10); // Match USB polling interval
    }
}

// TinyUSB HID callbacks
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

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)report;
    (void)len;
}