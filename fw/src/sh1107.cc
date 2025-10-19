/**
 * Copyright (c) 2021 John Robinson.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sh1107.h"

#include <hardware/i2c.h>
#include <pico/binary_info.h>
#include <pico/stdlib.h>
#include <stdlib.h>

static constexpr u32 kAddr = 0x3c;

static void write_i2c(const u8 *const buf, const u32 n, const bool is_cmd)
{
    u8 wr_buf[1024];
    assert(n + 1 <= sizeof(wr_buf));

    wr_buf[0] = is_cmd ? 0x00 : 0x40;
    __builtin_memcpy(wr_buf + 1, buf, n);
    const int ret = i2c_write_blocking(i2c_default, kAddr, wr_buf, n + 1, false);
    assert(ret == n + 1);
}

void srn_set_col_page(int col, int page)
{
    uint8_t buf[3];
    buf[0] = 0x10 | ((col >> 4) & 0x7);
    buf[1] = 0x00 | col & 0xF;
    buf[2] = 0xB0 | page & 0xf;
    write_i2c(buf, 3, true);
}

void srn_set_mem_adr_mode(int p_v)
{
    uint8_t buf[1];
    buf[0] = 0x20 | p_v & 1;
    write_i2c(buf, 1, true);
}

void srn_set_contrast(int contrast)
{
    uint8_t buf[2];
    buf[0] = 0x81;
    buf[1] = contrast & 0xFF;
    write_i2c(buf, 2, true);
}

void srn_set_seg_rot(int p_v)
{
    uint8_t buf[1];
    buf[0] = 0xa0 | p_v & 1;
    write_i2c(buf, 1, true);
}

void srn_turn_entire_disp_on(bool on)
{
    uint8_t buf[1];
    buf[0] = 0xA4;
    if (on)
        buf[0] |= 1;
    write_i2c(buf, 1, true);
}

void srn_set_reverse_display(bool reverse)
{
    uint8_t buf[1];
    buf[0] = 0xA6;
    if (reverse)
        buf[0] |= 1;
    write_i2c(buf, 1, true);
}

void srn_set_display_offset(int offset)
{
    uint8_t buf[2];
    buf[0] = 0x81;
    buf[1] = offset & 0x7F;
    write_i2c(buf, 2, true);
}

void srn_turn_display_on(bool on)
{
    uint8_t buf[1];
    buf[0] = 0xAE;
    if (on)
        buf[0] |= 1;
    write_i2c(buf, 1, true);
}

void srn_reverse_disp_on(bool reverse)
{
    uint8_t buf[1];
    buf[0] = 0xC0;
    if (reverse)
        buf[0] |= 8;
    write_i2c(buf, 1, true);
}

void srn_set_display_start(int start_line)
{
    uint8_t buf[2];
    buf[0] = 0xDB;
    buf[1] = start_line & 0x7F;
    write_i2c(buf, 2, true);
}

// PIXEL DATA
// The display pixel buffer is is a local copy of the display buffer
// in the SH1107.  All drawing commands make changes to display_pixel array
// and then call refresh to copy the holw buffer out to the SH1107.

u8 fb[16][128];

void srn_refresh()
{
    for (int j = 0; j < 16; j++)
    {
        srn_set_col_page(0, j);
        write_i2c(fb[j], 128, false);
    }
}

void srn_fast_clear()
{
    for (u32 j = 0; j < 16; j++)
    {
        for (u32 i = 0; i < 128; i++)
        {
            fb[j][i] = 0;
        }
    }
    srn_refresh();
}

// inits the I2C interface and clears the display
void sh1107_init()
{
    srn_turn_display_on(true);
    // srn_turn_entire_disp_on(true);
    // sleep_ms(1000);
    // srn_turn_entire_disp_on(false);
    // sleep_ms(500);

    // srn_fast_clear();
}
