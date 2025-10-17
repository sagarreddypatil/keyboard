#include "keyboard.h"

#include <bsp/board.h>
#include <pico/stdio_usb.h>
#include <stdlib.h>
#include <tusb.h>

Keyboard kb;

void hid_task()
{
    const u64 interval_us = 2000;
    static u64 start_us = 0;

    if (get_absolute_time() - start_us < interval_us)
    {
        return;
    }
    start_us += interval_us;

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

int main(void)
{
    board_init();
    tusb_init();

    stdio_usb_init();

    kb.init();
    while (1)
    {
        tud_task();
        hid_task();
    }

    return 0;
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
