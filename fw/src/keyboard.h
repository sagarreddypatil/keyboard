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

    u8 m_keycodes[6]{};
    u8 m_modifiers = 0;
};