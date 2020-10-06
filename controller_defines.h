
#ifndef CONTROLLER_DEFINES_H
#define CONTROLLER_DEFINES_H

/*
 * Collection of all Controller-specific stuff, most notably all descriptors
 * and the corresponding structs
 * Copied from Switch-Fightstick
 */

#include <stddef.h>
#include <stdint.h>

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/hid.h>

#include "usb/usb_addon.h"
#include "usb/usbstring.h"

//descriptors

#define CONFIG_VALUE 1

extern struct usb_config_descriptor config;
extern struct usb_interface_descriptor if_descriptor;
extern struct usb_hid_descriptor hid_descriptor;
extern struct usb_endpoint_descriptor ep_descriptor_in;
extern struct usb_endpoint_descriptor ep_descriptor_out;
extern struct usb_device_descriptor device_descriptor;

extern unsigned char hid_report[];
extern size_t hid_report_size;

void initDescriptors();

//Strings

enum {
		STRINGID_MANUFACTURER = 1,
		STRINGID_PRODUCT,
		STRINGID_MAX
};

extern struct usb_string stringtab[];
extern struct usb_gadget_strings strings;

// Sent and received data

typedef enum {
	SWITCH_Y       = 0x01,
	SWITCH_B       = 0x02,
	SWITCH_A       = 0x04,
	SWITCH_X       = 0x08,
	SWITCH_L       = 0x10,
	SWITCH_R       = 0x20,
	SWITCH_ZL      = 0x40,
	SWITCH_ZR      = 0x80,
	SWITCH_SELECT  = 0x100,
	SWITCH_START   = 0x200,
	SWITCH_LCLICK  = 0x400,
	SWITCH_RCLICK  = 0x800,
	SWITCH_HOME    = 0x1000,
	SWITCH_CAPTURE = 0x2000,
} JoystickButtons_t;

typedef struct {
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
	uint8_t  VendorSpec;
} USB_JoystickReport_Input_t;

typedef struct {
	uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
	uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
	uint8_t  LX;     // Left  Stick X
	uint8_t  LY;     // Left  Stick Y
	uint8_t  RX;     // Right Stick X
	uint8_t  RY;     // Right Stick Y
} USB_JoystickReport_Output_t;

extern const USB_JoystickReport_Input_t input_neutral;

#endif
