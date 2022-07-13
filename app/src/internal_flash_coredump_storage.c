#include <zephyr.h>

// #include <sys/util.h>

#include <stdlib.h>
// #include <string.h>
#include <drivers/flash.h>
#include <logging/log.h>
#include <soc.h>

#include "memfault/components.h"
#include "memfault/ports/buffered_coredump_storage.h"

LOG_MODULE_REGISTER(mflt_coredump_storage, LOG_LEVEL_INF);

#ifdef DT_CHOSEN_ZEPHYR_FLASH_CONTROLLER_LABEL
#define FLASH_DEV_NAME DT_CHOSEN_ZEPHYR_FLASH_CONTROLLER_LABEL
#else
#error "must have a flash contoller"
#endif

// On the STM32WL55, 256kB of flash from 0x0800_0000 to 0x0803_FFFF
#define FLASH_START_ADDR 0x08000000
#define MEMFAULT_COREDUMP_STORAGE_START_ADDR (0x08030000 - FLASH_START_ADDR)
#define MEMFAULT_COREDUMP_STORAGE_END_ADDR (0x08040000 - FLASH_START_ADDR)

// void memfault_platform_fault_handler(const sMfltRegState *regs,
//                                      eMemfaultRebootReason reason) {
//   memfault_stm32cubewb_flash_clear_ecc_errors(
//       MEMFAULT_COREDUMP_STORAGE_START_ADDR,
//       MEMFAULT_COREDUMP_STORAGE_END_ADDR, NULL);
// }

// Error writing to flash - should never happen & likely detects a
// configuration error. Call the reboot handler which will halt the device if
// a debugger is attached and then reboot.
// MEMFAULT_NO_OPT
// static void prv_coredump_writer_assert_and_reboot(int error_code) {
//   LOG_ERR("Error writing to flash, error_code: %d", error_code);
//   memfault_platform_halt_if_debugging();
//   memfault_platform_reboot();
// }

static bool prv_op_within_storage_bounds(uint32_t offset, size_t data_len) {
  sMfltCoredumpStorageInfo info = {0};
  memfault_platform_coredump_storage_get_info(&info);
  return (offset + data_len) <= info.size;
}

void memfault_platform_coredump_storage_clear(void) {
  int result = memfault_platform_coredump_storage_erase(
      MEMFAULT_COREDUMP_STORAGE_START_ADDR, 4);
  if (result != 0) {
    MEMFAULT_LOG_ERROR("Could not clear coredump storage %d", result);
  }
}

void memfault_platform_coredump_storage_get_info(
    sMfltCoredumpStorageInfo *info) {
  const size_t size =
      MEMFAULT_COREDUMP_STORAGE_END_ADDR - MEMFAULT_COREDUMP_STORAGE_START_ADDR;

  *info = (sMfltCoredumpStorageInfo){
      .size = size,
      .sector_size = 2048,  // hard coded, not used anywhere anyway
  };
}

bool memfault_platform_coredump_storage_buffered_write(
    sCoredumpWorkingBuffer *blk) {
  const uint32_t start_addr = MEMFAULT_COREDUMP_STORAGE_START_ADDR;
  const uint32_t addr = start_addr + blk->write_offset;

  uint64_t data;
  const struct device *flash_dev = device_get_binding(FLASH_DEV_NAME);

  for (size_t i = 0; i < MEMFAULT_COREDUMP_STORAGE_WRITE_SIZE;
       i += sizeof(data)) {
    memcpy(&data, &blk->data[i], sizeof(data));

    if (flash_write(flash_dev, addr + i, &data, sizeof(data)) != 0) {
      LOG_ERR("write error");
      // prv_coredump_writer_assert_and_reboot(res);
      return false;
    }

    uint64_t check_data = 0;
    (void)flash_read(flash_dev, addr + i, &check_data, sizeof(check_data));
    if (check_data != data) {
      LOG_ERR("readback error");
      // prv_coredump_writer_assert_and_reboot(res);
      return false;
    }
  }

  *blk = (sCoredumpWorkingBuffer){0};
  return true;
}

bool memfault_platform_coredump_storage_read(uint32_t offset, void *data,
                                             size_t read_len) {
  if (!prv_op_within_storage_bounds(offset, read_len)) {
    return false;
  }

  // The internal flash is memory mapped so we can just use memcpy!
  const uint32_t start_addr = MEMFAULT_COREDUMP_STORAGE_START_ADDR;
  memcpy(data, (void *)(start_addr + offset), read_len);
  return true;
}

bool memfault_platform_coredump_storage_erase(uint32_t offset,
                                              size_t erase_size) {
  const struct device *flash_dev = device_get_binding(FLASH_DEV_NAME);
  struct flash_pages_info info;

  // get starting offset
  int result = flash_get_page_info_by_offs(flash_dev, offset, &info);
  if (result != 0) {
    LOG_ERR("Could not get flash page info for offset 0x%x", offset);
    return false;
  }

  // get ending offset
  size_t start_offset = info.start_offset;
  offset = offset + erase_size;
  result = flash_get_page_info_by_offs(flash_dev, offset, &info);
  if (result != 0) {
    LOG_ERR("Could not get flash page info for offset 0x%x", offset);
    return false;
  }

  // total erase size
  size_t size = info.start_offset + info.size;

  // perform the erase
  LOG_INF("erasing flash pages 0x%x-0x%x", start_offset, start_offset + size);

  result = flash_erase(flash_dev, start_offset, size);

  if (result != 0) {
    LOG_ERR("Could not erase flash pages 0x%x-0x%x", start_offset,
            start_offset + size);
    return false;
  }

  return true;
}
