#include "keyboard.h"

#include "config.h"

void Keyboard::init()
{
    // Initialize pins.
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

    // Ensure time_micros() is never less than the debounce interval.
    while (time_micros() < kDebounceIntervalUs)
        tight_loop_contents();
}

bool Keyboard::scan()
{
    // Scan.
    bool key_states[kRows][kCols]{};
    for (u32 ci = 0; ci < kCols; ++ci)
    {
        const u32 col = kColPins[ci];
        gpio_put(col, true);
        sleep_us(1);
        for (u32 ri = 0; ri < kRows; ++ri)
        {
            const u32 row = kRowPins[ri];
            key_states[ri][ci] = gpio_get(row);
        }
        gpio_put(col, false);
    }

    // Debounce.
    const u64 ts = time_micros();
    for (u32 ci = 0; ci < kCols; ++ci)
    {
        for (u32 ri = 0; ri < kRows; ++ri)
        {
            bool &last_state = m_last_states[ri][ci];
            bool &state = key_states[ri][ci];
            u64 &last_state_time = m_last_state_times[ri][ci];

            if (UNLIKELY(state != last_state && ts < last_state_time + kDebounceIntervalUs))
                state = last_state;
            if (UNLIKELY(state != last_state))
                last_state_time = ts;
            last_state = state;
        }
    }

    // Populate outputs.
    u32 i = 0;
    m_modifiers = 0;
    const bool fn = key_states[kFnRow][kFnCol];
    bool any_pressed = false;
    for (u32 ri = 0; ri < kRows; ++ri)
    {
        for (u32 ci = 0; ci < kCols; ++ci)
        {
            const u8 kc = fn ? kFnKeymap[ri][ci] : kKeymap[ri][ci];
            const char *const str = kKeymapStr[ri][ci];
            if (LIKELY(!key_states[ri][ci] || !kc))
            {
                continue;
            }
            // printf("key fn=%d row=%d col=%d name=%s\n", fn, ri, ci, kKeymapStr[ri][ci]);
            if (kc >= HID_KEY_CONTROL_LEFT && kc <= HID_KEY_GUI_RIGHT)
            {
                any_pressed |= true;
                m_modifiers |= (1 << (kc - HID_KEY_CONTROL_LEFT));
            }
            else if (i < ARRAY_SIZE(m_keycodes))
            {
                any_pressed |= true;
                m_keycodes[i++] = kc;
            }
        }
    }

    for (; i < ARRAY_SIZE(m_keycodes); ++i)
        m_keycodes[i] = 0;

    // Return.
    const bool ret = any_pressed || (m_prev_any && !any_pressed);
    m_prev_any = any_pressed;
    return ret;
}
