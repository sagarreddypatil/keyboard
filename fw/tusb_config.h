#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

// defined by compiler flags for flexibility
#ifndef CFG_TUSB_MCU
#define CFG_TUSB_MCU OPT_MCU_RP2040
#endif

#ifndef CFG_TUSB_OS
#define CFG_TUSB_OS OPT_OS_NONE
#endif

#ifndef CFG_TUSB_DEBUG
#define CFG_TUSB_DEBUG 0
#endif

// Enable Device stack
#define CFG_TUD_ENABLED 1

// RHPort number used for device
#define BOARD_TUD_RHPORT 0

// RHPort max operational speed can defined by board.mk
#ifndef BOARD_TUD_MAX_SPEED
#define BOARD_TUD_MAX_SPEED OPT_MODE_DEFAULT_SPEED
#endif

// Device mode with rhport and speed defined by board.mk
#if BOARD_TUD_RHPORT == 0
#define CFG_TUSB_RHPORT0_MODE (OPT_MODE_DEVICE | BOARD_TUD_MAX_SPEED)
#else
#define CFG_TUSB_RHPORT1_MODE (OPT_MODE_DEVICE | BOARD_TUD_MAX_SPEED)
#endif

// Default is max speed that hardware controller could support with on-chip PHY
#define CFG_TUD_MAX_SPEED BOARD_TUD_MAX_SPEED

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#ifndef CFG_TUD_ENDPOINT0_SIZE
#define CFG_TUD_ENDPOINT0_SIZE 64
#endif

//------------- CLASS -------------//
#define CFG_TUD_HID 1
#define CFG_TUD_CDC 0
#define CFG_TUD_MSC 0
#define CFG_TUD_MIDI 0
#define CFG_TUD_VENDOR 0

// HID buffer size Should be sufficient to hold ID (if any) + Data
#define CFG_TUD_HID_EP_BUFSIZE 16

#ifdef __cplusplus
}
#endif

#endif /* _TUSB_CONFIG_H_ */