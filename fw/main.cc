#include "config.h"
#include "util.h"

#include <bsp/board.h>
#include <hardware/gpio.h>
#include <stdlib.h>
#include <tusb.h>

class Keyboard
{
public:
    Keyboard()
    {
    }
    Keyboard(const Keyboard &) = delete;
    Keyboard(Keyboard &&) = delete;

    void init()
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

    void scan()
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
                        const u8 kc = kKeymap[ri][ci];
                        printf("key row=%d col=%d name=%s\n", ri, ci, kKeymapStr[ri][ci]);

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

    bool keys_changed() const
    {
        if (m_modifiers != m_prev_modifiers)
            return true;

        for (u32 i = 0; i < ARRAY_SIZE(m_keycodes); ++i)
        {
            if (m_keycodes[i] != m_prev_keycodes[i])
                return true;
        }
        return false;
    }

    bool keys_pressed() const
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

    void next()
    {
        m_prev_modifiers = m_modifiers;
        m_modifiers = 0;
        for (u32 i = 0; i < ARRAY_SIZE(m_keycodes); ++i)
        {
            m_prev_keycodes[i] = m_keycodes[i];
            m_keycodes[i] = 0;
        }
    }

    u8 m_keycodes[6]{};
    u8 m_modifiers = 0;

private:
    u8 m_prev_keycodes[6]{};
    u8 m_prev_modifiers = 0xFF;

    static_assert(ARRAY_SIZE(m_keycodes) == ARRAY_SIZE(m_prev_keycodes));
};

Keyboard kb;

void hid_task();

int main(void)
{
    board_init();
    tusb_init();

    kb.init();
    while (1)
    {
        tud_task();
        hid_task();
    }

    return 0;
}

static void send_hid_report()
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
    {
        return;
    }

    if (kb.keys_pressed())
    {
        tud_hid_keyboard_report(0x01, kb.m_modifiers, kb.m_keycodes);
    }
    else
    {
        // send empty key report if previously has key pressed
        if (kb.keys_changed())
        {
            tud_hid_keyboard_report(0x01, 0, NULL);
        }
    }
}

void hid_task()
{
    // Poll every 10ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < interval_ms)
    {
        return; // not enough time
    }
    start_ms += interval_ms;

    // Check for keys pressed
    kb.scan();
    bool const keys_pressed = kb.keys_pressed();

    // Remote wakeup
    if (tud_suspended() && keys_pressed)
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }
    else
    {
        // send a report
        send_hid_report();
    }
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len)
{
    (void)instance;
    (void)len;
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

void tud_mount_cb(void)
{
}

void tud_umount_cb(void)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
}

void tud_resume_cb(void)
{
}
