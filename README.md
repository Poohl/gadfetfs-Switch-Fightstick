
# Controlling the Nintendo-Switch through GadgetFS

## What is this

A port of Switch-Fightstick to GadgetFS, a userspace Linux usb gadget driver including an interative terminal to generate the input.

I mainly developed this to use a Wiimote on the switch without buying anything.

I tested this on a Raspi4B 1GB, connected to both, a USB charger for power and a launch Switch Dock, using a usb Y-cable (2x Male to 1x Female. Only one Male port has data pins) and a new Switch (HAC-001-01) running firmware 10.1.0.

## What's it do yet?

Functionallity making use of this controller-emulation is implemented as commands. Go to [commands](./commands/) for more details.

If you want to write your own stuff, look at [input_dummy](input_dummy) to see the interface you should implement. The dummy just presses A each second.

## Installation

0. Clone this repo
```
git clone [this repo's url]
```
1. Enable dwc, (might not be neccessary) & reboot
1. mount gadgetfs
```
sudo ./mountFadgetFS.sh
```
1. Figure out your controllername
The name of your controller and it's endpoints might differ from what is used in `main.c`. To check your's look into `/dev/gadget/` and adjust `ep0_path` to the "file" you find there (replace the `fe980000.usb`, if you have multiple slave-controllers, there will be multiple files. pick one you know the corresponding port for)
1. Compiling  
First see [commands](./commands/) to view the availabe commands and uncomment anything you don't want in the Makefile (and install xwiimote if you want wiimote-support), then run
```
make
```
1. You're done. Now run `sudo ./gadgetfs_controller.bin` to enable the controller emulation. I recommend plugging the usb into the switch either before or shortly after this step, to prevent linux going into some kind of powersave.
1. Wrong endpoint names  
If you see an error similar to
```
Error: ep = open(path, O_RDWR) was -19 not >= 0; errno = no sutch file or directory
```
your driver might use other endpoint names. Run `sudo ls /dev/gadget` (and connect the raspi to some USB-Host) to view the list of all files generated by gadgetFS. Set `ep1_in_path` and `ep2_out_path` to the corresponding filepaths (in `main.c`).
## Resources
This Project is in large parts based on or was created using the following resources:
* Comprehensive Gadgetfs-tutorial with a minimal example working on a raspi: https://blog.soutade.fr/post/2016/07/create-your-own-usb-gadget-with-gadgetfs.html
* The original Switch-Fightstick: https://github.com/progmem/Switch-Fightstick
* It's arduino port: https://github.com/shinyquagsire23/Switch-Fightstick
* The GadgetFS refrence example and additional sources: http://www.linux-usb.org/gadget/
	- http://www.linux-usb.org/gadget/usb.c
	- http://www.linux-usb.org/gadget/usbstring.c
	- http://www.linux-usb.org/gadget/usbstring.h
* a GadgetFS implemetaion of HID-Devices (mouse and keyboard): https://sourceforge.net/projects/hid-gadgetfs/
* Website for testing the controller: https://gamepad-tester.com/
* `lsusb`, `wireshark`, `strace`