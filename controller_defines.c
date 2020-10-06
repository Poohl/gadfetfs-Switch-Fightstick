
#include "controller_defines.h"

#include <stdio.h>

struct usb_config_descriptor config;
struct usb_interface_descriptor if_descriptor;
struct usb_hid_descriptor hid_descriptor;
struct usb_endpoint_descriptor ep_descriptor_in;
struct usb_endpoint_descriptor ep_descriptor_out;
struct usb_device_descriptor device_descriptor;

/*
 * generated from the one used in the original project by using xxd -i
 * This is the one with Home and capture
 */
unsigned char hid_report[] = {
	0x05, 0x01, 0x09, 0x05, 0xa1, 0x01, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00,
	0x45, 0x01, 0x75, 0x01, 0x95, 0x10, 0x05, 0x09, 0x19, 0x01, 0x29, 0x10,
	0x81, 0x02, 0x05, 0x01, 0x25, 0x07, 0x46, 0x3b, 0x01, 0x75, 0x04, 0x95,
	0x01, 0x65, 0x14, 0x09, 0x39, 0x81, 0x42, 0x65, 0x00, 0x95, 0x01, 0x81,
	0x01, 0x26, 0xff, 0x00, 0x46, 0xff, 0x00, 0x09, 0x30, 0x09, 0x31, 0x09,
	0x32, 0x09, 0x35, 0x75, 0x08, 0x95, 0x04, 0x81, 0x02, 0x06, 0x00, 0xff,
	0x09, 0x20, 0x95, 0x01, 0x81, 0x02, 0x0a, 0x21, 0x26, 0x95, 0x08, 0x91,
	0x02, 0xc0
};
size_t hid_report_size = sizeof(hid_report);

struct usb_string stringtab [] = {
		{ STRINGID_MANUFACTURER, "HORI CO.,LTD.", },
		{ STRINGID_PRODUCT,      "POKKEN CONTROLLER", },
		{ STRINGID_MAX,          NULL},
};

struct usb_gadget_strings strings = {
		.language = 0x0409, /* en-us */
		.strings = stringtab,
};

const USB_JoystickReport_Input_t input_neutral = {0, 0x88, 128, 128, 128, 128, 0};

/*
 * This initiallizes all the descriptors to the known values, unless others are
 * specified by USB.
 * See HORI_Descriptors for the original.
 */
void initDescriptors() {
	device_descriptor.bLength = USB_DT_DEVICE_SIZE;
	device_descriptor.bDescriptorType = USB_DT_DEVICE;
	device_descriptor.bcdUSB = 0x0200; // USB 2
	device_descriptor.bDeviceClass = USB_CLASS_PER_INTERFACE;
	device_descriptor.bDeviceSubClass = 0;
	device_descriptor.bDeviceProtocol = 0;
	//device_descriptor.bMaxPacketSize0 = 255; Set by driver
	device_descriptor.idVendor = 0x0f0d; // HORI CO.,LTD.
	device_descriptor.idProduct = 0x0092; // POKKEN CONTROLLER
	device_descriptor.bcdDevice = 0x0100; // V 1.00
	// Strings
	device_descriptor.iManufacturer = STRINGID_MANUFACTURER;
	device_descriptor.iProduct = STRINGID_PRODUCT;
	device_descriptor.iSerialNumber = 0; // NULL
	device_descriptor.bNumConfigurations = 1; // Only one configuration

	ep_descriptor_in.bLength = USB_DT_ENDPOINT_SIZE;
	ep_descriptor_in.bDescriptorType = USB_DT_ENDPOINT;
	ep_descriptor_in.bEndpointAddress = USB_DIR_IN | 1;
	ep_descriptor_in.bmAttributes = USB_ENDPOINT_XFER_INT;
	ep_descriptor_in.wMaxPacketSize = 64;
	ep_descriptor_in.bInterval = 5;

	ep_descriptor_out.bLength = USB_DT_ENDPOINT_SIZE;
	ep_descriptor_out.bDescriptorType = USB_DT_ENDPOINT;
	ep_descriptor_out.bEndpointAddress = USB_DIR_OUT | 2;
	ep_descriptor_out.bmAttributes = USB_ENDPOINT_XFER_INT;
	ep_descriptor_out.wMaxPacketSize = 64;
	ep_descriptor_out.bInterval = 5;

	hid_descriptor.bLength = sizeof(hid_descriptor);
	hid_descriptor.bDescriptorType = HID_DT_HID;
	hid_descriptor.bcdHID = 0x0111;
	hid_descriptor.bCountryCode = 0;
	hid_descriptor.bNumDescriptors = 1;
	hid_descriptor.desc[0].bDescriptorType = HID_DT_REPORT;
	hid_descriptor.desc[0].wDescriptorLength = sizeof(hid_report);

	//printf("hid report size: %d\n", sizeof(hid_report));

	if_descriptor.bLength = sizeof(if_descriptor);
	if_descriptor.bDescriptorType = USB_DT_INTERFACE;
	if_descriptor.bInterfaceNumber = 0;
	if_descriptor.bAlternateSetting = 0;
	if_descriptor.bNumEndpoints = 2;
	if_descriptor.bInterfaceClass = USB_CLASS_HID;
	if_descriptor.bInterfaceSubClass = 0;
	if_descriptor.bInterfaceProtocol = 0;
	if_descriptor.iInterface = 0;

	config.bLength = sizeof(config);
	config.bDescriptorType = USB_DT_CONFIG;
	config.wTotalLength = config.bLength + if_descriptor.bLength
				+ hid_descriptor.bLength + ep_descriptor_in.bLength + ep_descriptor_out.bLength;
	config.bNumInterfaces = 1;
	config.bConfigurationValue = CONFIG_VALUE;
	config.iConfiguration = 0;
	config.bmAttributes = 0x80; // TODO: What does this mean?
	config.bMaxPower = 500 >> 1; // 500mA
}
