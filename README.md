# Example Memfault Zephyr application

Based on https://github.com/zephyrproject-rtos/example-application , this
provides a minimal reference for Memfault integration.

This should work on any supported target board*

*For the NXP RT10xx EVKs, use the following branch instead (tested on RT1021 and
RT1060):

- https://github.com/memfault/zephyr-memfault-example/tree/rt102x-noinit-workaround

## Usage

After setting up a zephyr development environment
(https://docs.zephyrproject.org/latest/getting_started/index.html), you can run
the following commands to test the application:

```shell
# intialize this project
❯ west init -m https://github.com/memfault/zephyr-memfault-example.git zephyr-memfault-example
❯ cd zephyr-memfault-example
❯ west update

# build- example here is Nordic nRF52480 dk
❯ west build -b nrf9160dk_nrf9160_ns -s zephyr-memfault-example/app

# connect the USB cable to the board

# open a console in one terminal
❯ pyserial-miniterm /dev/serial/by-id/usb-SEGGER_J-Link_000683232543-if00 115200 --raw

# now flash from another terminal
❯ west flash

# the console should start working
uart:~$ mflt get_device_info
<inf> <mflt>: S/N: DEMOSERIAL
<inf> <mflt>: SW type: zephyr-app
<inf> <mflt>: SW version: 1.0.0-dev
<inf> <mflt>: HW version: nrf52840dk_nrf52840
```
