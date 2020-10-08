
ccflags = -I. -g -Wall -finline-functions -finline-small-functions
ldflags = -lpthread
featureflags =

src = main.c gadgetfs_controller.c controller_defines.c common.c usb/usbstring.c util.c

# dummy input
#src += input_dummy.c

# commands
src += input_commands/commands.c

# click
src +=  input_commands/click.c
featureflags += -DCLICK

# wii
src +=  input_commands/wii.c
ldflags += -lxwiimote
featureflags += -DWII

# mariomaker2
src += input_commands/mariomaker2.c
featureflags += -DMARIOMAKER2

# tas
src += input_commands/tas.c
featureflags += -DTAS

all: gadgetfs_controller.bin

run: gadgetfs_controller.bin
	sudo ./gadgetfs_controller.bin

run_tmp: gadgetfs_controller.bin
	cp $< /tmp/
	sudo /tmp/$<

.PHONY: gadgetfs_controller.bin
gadgetfs_controller.bin:
	gcc $(ccflags) $(featureflags) -o $@ $(src) $(ldflags)

clear:
	rm gadgetfs_controller.bin
