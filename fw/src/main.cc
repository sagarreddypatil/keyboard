#include "keyboard.h"
#include "sh1107.h"

#include <bsp/board.h>
#include <hardware/i2c.h>
#include <pico/multicore.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <stdlib.h>
#include <tusb.h>

Keyboard kb;

static u64 time_micros()
{
    return to_us_since_boot(get_absolute_time());
}

void hid_task()
{
    const u64 kIntervalMicros = 2000;
    static u64 start_us = 0;

    const u64 t = time_micros();
    if (t < start_us)
    {
        return;
    }
    start_us = t + kIntervalMicros;

    kb.scan();
    if (tud_suspended())
    {
        tud_remote_wakeup();
    }
    else if (tud_hid_ready())
    {
        tud_hid_keyboard_report(0x01, kb.m_modifiers, kb.m_keycodes);
    }
}

void fb_task()
{
    u8 *buf = &fb[0][0];
    while (getchar() != 0)
        ;
    for (u32 i = 0; i < sizeof(fb); ++i)
    {
        u8 val = getchar();
        if (val == 0)
            break;
        if (val == 1)
        {
            val = ~u8(getchar());
            if (val == 0xff)
                break;
        }
        buf[i] = val;
    }
}

void display_task()
{
    srn_refresh();
}

static constexpr u32 kOledSda = 24;
static constexpr u32 kOledScl = 25;

void core1_entry();

int main(void)
{
    board_init();
    tusb_init();

    stdio_usb_init();
    multicore_launch_core1(core1_entry);

    kb.init();
    while (1)
    {
        tud_task();
        hid_task();
    }

    return 0;
}

void core1_entry()
{
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(kOledSda, GPIO_FUNC_I2C);
    gpio_set_function(kOledScl, GPIO_FUNC_I2C);
    gpio_pull_up(kOledSda);
    gpio_pull_up(kOledScl);

    sh1107_init();
    srn_set_mem_adr_mode(0);
    while (1)
    {
        fb_task();
        display_task();
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
