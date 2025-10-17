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
                    const bool fn = ri == kFnRow && ci == kFnCol;
                    printf("key fn=%d row=%d col=%d name=%s\n", fn, ri, ci, kKeymapStr[ri][ci]);

                    const u8 kc = fn ? kFnKeymap[ri][ci] : kKeymap[ri][ci];
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
        }
        gpio_put(col, false);
    }
    for (; i < ARRAY_SIZE(m_keycodes); ++i)
    {
        m_keycodes[i] = 0;
    }
}

bool Keyboard::keys_pressed() const
{
    if (m_modifiers)
    {
        return true;
    }
    for (u32 i = 0; i < ARRAY_SIZE(m_keycodes); ++i)
    {
        if (m_keycodes[i] != 0)
            return true;
    }
    return false;
}
