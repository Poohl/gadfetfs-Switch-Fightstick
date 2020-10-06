# Commands

This is a implementation of `input_gernerator.h` featuring an interactive command prompt to control the sent inputs.

## Compiling

All used commands are included if the corresponding flag is set by the Makefile.

By default all (working) commands are turned on.

## Available Commands

### click {buttons}

given a space-seperated list of Buttons, this command holds those down for 10 frames.
useful for navigating menus.

### wii [horizontal|vertical]

using xwiimote a connected wiimote is captured and used as input (nunchuck is supported).  
The command toggles the functionality, run wii again to disable it.  
To install xwiimote see https://github.com/dvdhrm/xwiimote. Note that the debian package (`xwiimote2`) seems to lack the library headers, so you can't use it for compilation.

### mariomaker2 [3d_spin|sprint|inv_sprint]

- 3d_spin causes you to automatically execute a spin-jump when pressing L or R
- sprint or inv_sprint inverts x and y

The command toggles the functionality, run mariomaker2 [3d_spin|sprint|inv_sprint] again to disable it.
