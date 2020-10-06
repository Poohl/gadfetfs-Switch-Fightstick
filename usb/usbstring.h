/* From usbstring.[ch] */
#ifndef USBSTRING_H
#define USBSTRING_H

#include <sys/types.h>
#include <linux/usb/ch9.h>

#include <string.h>
#include <errno.h>

struct usb_string {
	__u8			id;
	const char		*s;
};

struct usb_gadget_strings {
	__u16			language;	/* 0x0409 for en-us */
	struct usb_string	*strings;
};

int usb_gadget_get_string (struct usb_gadget_strings *table, int id, __u8 *buf);

#endif
