#pragma once

#include "config.h"
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

    // True if any(keys pressed), or if falling edge on any(keys pressed).
    bool scan();

    // Inform that we've actually used a scan.
    void commit();

    bool m_fn = false;

    u8 m_keycodes[6]{};
    u8 m_modifiers = 0;
    bool m_prev_any = false;
    bool m_cur_any;

    bool m_last_states[kRows][kCols]{};
    u64 m_last_state_times[kRows][kCols]{};
};