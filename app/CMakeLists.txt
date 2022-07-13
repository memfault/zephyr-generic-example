#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C VERSION 1.0.0)

target_sources(app PRIVATE
    src/main.c
    ${ZEPHYR_MEMFAULT_FIRMWARE_SDK_MODULE_DIR}/ports/stm32cube/wl/rcc_reboot_tracking.c
    src/internal_flash_coredump_storage.c
    # /home/noah/dev/memfault/memfault/sdk/embedded/ports/stm32cube/wl/flash_coredump_storage.c
)
