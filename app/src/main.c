#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/zephyr.h>

#include "memfault/components.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

// Blink code taken from the zephyr/samples/basic/blinky example.
static void blink_forever(void) {
#if CONFIG_QEMU_TARGET
  k_sleep(K_FOREVER);
#else
/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

  static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

  int ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
  if (ret < 0) {
    return;
  }

  while (1) {
    gpio_pin_toggle_dt(&led);
    k_msleep(SLEEP_TIME_MS);
  }
#endif  // CONFIG_QEMU_TARGET
}

#include <malloc.h>
void memfault_metrics_heartbeat_collect_data(void) {
  const struct mallinfo mi = mallinfo();
  const uint32_t heap_used_bytes = mi.uordblks;
  memfault_metrics_heartbeat_set_unsigned(MEMFAULT_METRICS_KEY(HeapUsedBytes),
                                          heap_used_bytes);
}

void memfault_platform_get_device_info(sMemfaultDeviceInfo *info) {
  *info = (sMemfaultDeviceInfo){
      .device_serial = "DEMOSERIAL",
      .software_type = "zephyr-app",
      .software_version = "1.0.0-dev",
      .hardware_version = CONFIG_BOARD,
  };
}

MEMFAULT_NORETURN
void memfault_platform_reboot(void) {
  memfault_platform_halt_if_debugging();

  // Note: There is no header exposed for this zephyr function
  extern void sys_arch_reboot(int type);

  sys_arch_reboot(0);
  CODE_UNREACHABLE;
}

#include <stdio.h>

void main(void) {
  char *heap = malloc(128);
  if (!heap) {
    LOG_ERR("Failed to allocate heap");
  } else {
    LOG_INF("Allocated heap at %p", heap);
    sprintf(heap, "allocated on heap");
    LOG_INF("Hello World! %s", heap);

    LOG_INF("Leaking heap allocation");
  }
  LOG_INF("Memfault Demo App! Board %s\n", CONFIG_BOARD);
  memfault_device_info_dump();
  blink_forever();
}
