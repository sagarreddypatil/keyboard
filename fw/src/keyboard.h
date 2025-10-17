#pragma once

#include "util.h"

#include <hardware/gpio.h>
#include <pico/time.h>

class Keyboard
{
public:
    Keyboard()
    {
    }
    Keyboard(const Keyboard &) = delete;
    Keyboard(Keyboard &&) = delete;

    void init();
    void scan();
    bool keys_changed() const;
    bool keys_pressed() const;
    void next();

    u8 m_keycodes[6]{};
    u8 m_modifiers = 0;

private:
    u8 m_prev_keycodes[6]{};
    u8 m_prev_modifiers = 0xFF;

    static_assert(ARRAY_SIZE(m_keycodes) == ARRAY_SIZE(m_prev_keycodes));
};