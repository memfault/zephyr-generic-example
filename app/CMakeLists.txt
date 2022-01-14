#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app LANGUAGES C VERSION 1.0.0)

target_sources(app PRIVATE src/main.c)

# Add the stm32f7 reboot reason impl. for testing
target_sources(
    app
    PRIVATE
    $ENV{ZEPHYR_BASE}/../modules/lib/memfault-firmware-sdk/ports/stm32cube/f7/rcc_reboot_tracking.c
)
