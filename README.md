# Example Memfault Zephyr application

Based on https://github.com/zephyrproject-rtos/example-application , this
provides a minimal reference for Memfault integration.

This branch has a specific workaround for the RT10xx boards. Tested on the
following EVKs:

- MIMXRT1020-EVK
- MIMXRT1060-EVKB

## Usage

After setting up a zephyr development environment
(https://docs.zephyrproject.org/latest/getting_started/index.html), you can run
the following commands to test the application:

```shell
# intialize this project
❯ west init --manifest-url https://github.com/memfault/zephyr-memfault-example \
  --manifest-rev rt102x-noinit-workaround zephyr-memfault-example
❯ cd zephyr-memfault-example
❯ west update

# build- example here is NXP MIMXRT1060_EVK
❯ west build -b mimxrt1060_evk zephyr-memfault-example/app -- -DCONFIG_BUILD_OUTPUT_HEX=y

# connect the USB cable to the board

# open a console in one terminal
❯ pyserial-miniterm --raw /dev/ttyACM0 115200 --raw

# now flash from another terminal. note: pyocd seems to work better than j-link
❯ west flash --runner pyocd

# the console should start working
uart:~$ mflt get_device_info
<inf> <mflt>: S/N: DEMOSERIAL
<inf> <mflt>: SW type: zephyr-app
<inf> <mflt>: SW version: 1.0.0-dev
<inf> <mflt>: HW version: mimxrt1060_evk
```
