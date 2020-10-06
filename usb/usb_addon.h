
#ifndef USB_ADDON_H
#define USB_ADDON_H

/*
 * Stuff that in some way is somewhere in the linux-headers but i
 * couldn't find or is to much of a hassle to get.
 */

#include <stdbool.h>
#include <linux/types.h>

struct gadgetfs_hardware {
	 char* ep0_path;
	 char* ep1_in_path;
	 char* ep2_out_path;
	 bool highspeed;
};

struct usb_hid_class_descriptor {
	__u8 bDescriptorType;
	__le16 wDescriptorLength;
} __attribute__ ((packed));

struct usb_hid_descriptor {
	__u8 bLength;
	__u8 bDescriptorType;
	__le16 bcdHID;
	__u8 bCountryCode;
	__u8 bNumDescriptors;

	struct usb_hid_class_descriptor desc[1];
} __attribute__ ((packed));

#endif
