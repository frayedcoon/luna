`luna` is an experimental kernel written in C. Mostly it's for the
self-educational purpose.

# Prerequisites

## Supported devices

- stm32f3:
    - `NUCLEO STM32f303RE`
    - `NUCLEO STM32f303K8`

## Tools

- `arm-none-eabi-gcc` (in Ubuntu `gcc-arm-none-eabi`) :

build toolchain

- `minicom`:

for testing via debug terminal

- `stlink` ([Ubuntu users need to install from source](
https://github.com/texane/stlink/blob/master/doc/compiling.md)) :

for writing embedded flash memory

# Build instructions

Just call `make`.

The kernel binary `luna.bin` for target `BOARD=nucleo-stm32f303K8`
(default target) will be generated under `build/` directory.

If you want to build kernel for another platform, you should specify `BOARD`
variable for `make`.

Supported boards could be found inside `board/` folder.

Example:
```
    make BOARD=nucleo-stm32f303RE
```

## Build targets

- all :
create build directory and build the kernel binary

- flash :
build the kernel binary  and flash it to the device

- test :
build, flash the kernel binary and launch minicom on `/dev/ttyACMO`

    NOTE: device is configurable via `SERIAL_DEVICE` variable, example:
```
    make test SERIAL_DEVICE=/dev/ttyUSB0
```

- clean :
delete build directory

NOTE: non-default target board SHOULD be specified every call to `make`

## Build options

All configurable build options can be found in `user_vars.mk`

- CROSS_TOOL :
host cross-compiler prefix

- SERIAL_DEVICE :
debug tty device

- BOARD :
target board
