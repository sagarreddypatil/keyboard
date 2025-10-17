#include "keyboard.h"

#include "config.h"

void Keyboard::init()
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

void Keyboard::scan()
{
    u32 i = 0;
    m_modifiers = 0;

    bool pressed[kRows][kCols]{};

    for (u32 ci = 0; ci < kCols; ++ci)
    {
        const u32 col = kColPins[ci];
        gpio_put(col, true);
        sleep_us(1);
        {
            for (u32 ri = 0; ri < kRows; ++ri)
            {
                const u32 row = kRowPins[ri];
                pressed[ri][ci] = gpio_get(row);
            }
        }
        gpio_put(col, false);
    }
    const bool fn = pressed[kFnRow][kFnCol];
    for (u32 ri = 0; ri < kRows; ++ri)
    {
        for (u32 ci = 0; ci < kCols; ++ci)
        {
            if (!pressed[ri][ci])
            {
                continue;
            }

            printf("key fn=%d row=%d col=%d name=%s\n", fn, ri, ci, kKeymapStr[ri][ci]);
            const u8 kc = fn ? kFnKeymap[ri][ci] : kKeymap[ri][ci];
            if (!kc)
            {
                continue;
            }
            if (kc >= HID_KEY_CONTROL_LEFT && kc <= HID_KEY_GUI_RIGHT)
            {
                m_modifiers |= (1 << (kc - HID_KEY_CONTROL_LEFT));
            }
            else if (i < ARRAY_SIZE(m_keycodes))
            {
                m_keycodes[i++] = kc;
            }
        }
    }
    for (; i < ARRAY_SIZE(m_keycodes); ++i)
    {
        m_keycodes[i] = 0;
    }
}
