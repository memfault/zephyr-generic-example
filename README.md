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
❯ west build -b nrf52840dk_nrf52840 zephyr-memfault-example/app

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

For testing without real hardware, a `qemu` target can be used, for example:

```bash
# highly recommend '-DCONFIG_QEMU_ICOUNT=n' otherwise the guest runs too fast
❯ west build -b qemu_cortex_m3 --pristine=always zephyr-memfault-example/app -- -DCONFIG_QEMU_ICOUNT=n
❯ west build -t run

*** Booting Zephyr OS build zephyr-v3.1.0  ***
[00:00:00.000,000] <inf> mflt: GNU Build ID: a3f2f5da83bc62ceb0351f88a8b30d5cdab59ae9
[00:00:00.000,000] <inf> main: Memfault Demo App! Board qemu_cortex_m3

[00:00:00.000,000] <inf> mflt: S/N: DEMOSERIAL
[00:00:00.000,000] <inf> mflt: SW type: zephyr-app
[00:00:00.000,000] <inf> mflt: SW version: 1.0.0-dev
[00:00:00.000,000] <inf> mflt: HW version: qemu_cortex_m3
```
