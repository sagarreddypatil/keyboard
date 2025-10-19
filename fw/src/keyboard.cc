#include "keyboard.h"

#include "config.h"

template <u32 N>
class Ringbuffer
{
public:
    Ringbuffer() = default;

    void push(const u8 val)
    {
        if ((m_tail + 1) % N == m_head)
        {
            m_head = (m_head + 1) % N;
        }

        m_v[m_tail++] = val;
        m_tail %= N;
    }

    u32 size() const
    {
        if (m_tail >= m_head)
            return m_tail - m_head;
        else
            return N - (m_head - m_tail);
    }

    void contents(char *dst) const
    {
        for (u32 i = 0; i < size(); ++i)
        {
            dst[i] = m_v[(m_head + i) % N];
        }
    }

private:
    u32 m_head = 0;
    u32 m_tail = 0;
    u8 m_v[N];
};

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

    static bool prev_pressed[kRows][kCols]{};
    bool pressed[kRows][kCols]{};
    static Ringbuffer<7> cheese;

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
            const u8 kc = fn ? kFnKeymap[ri][ci] : kKeymap[ri][ci];
            const char *const str = kKeymapStr[ri][ci];
            if (!pressed[ri][ci] || !kc)
            {
                continue;
            }
            // printf("key fn=%d row=%d col=%d name=%s\n", fn, ri, ci, kKeymapStr[ri][ci]);
            if (kc >= HID_KEY_CONTROL_LEFT && kc <= HID_KEY_GUI_RIGHT)
            {
                m_modifiers |= (1 << (kc - HID_KEY_CONTROL_LEFT));
            }
            else if (i < ARRAY_SIZE(m_keycodes))
            {
                m_keycodes[i++] = kc;
            }
            if (!prev_pressed[ri][ci] && __builtin_strlen(str) == 1)
            {
                const char chr = str[0];
                cheese.push(chr);
                printf("pushing %s\n", str);
                char buf[7]{};
                cheese.contents(buf);
                printf("size: %d contents %s\n", cheese.size(), buf);
                if (__builtin_memcmp(buf, "CHEESE", 6) == 0)
                {
                    printf("cheese detected!!\n");
                    m_modifiers = (1 << (HID_KEY_ALT_LEFT - HID_KEY_CONTROL_LEFT));
                    m_keycodes[0] = HID_KEY_BACKSPACE;
                    i = 1;
                }
            }
        }
    }
    for (; i < ARRAY_SIZE(m_keycodes); ++i)
    {
        m_keycodes[i] = 0;
    }

    for (u32 ri = 0; ri < kRows; ++ri)
    {
        for (u32 ci = 0; ci < kCols; ++ci)
        {
            prev_pressed[ri][ci] = pressed[ri][ci];
        }
    }
}
