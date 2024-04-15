#include <stdio.h>

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_demo.h"
#include "spi_demo.h"
#include "uart_demo.h"

void app_main() {
  spi_demo_start();
  i2c_demo_start();
  uart_demo_start();

  // let the console initialize before printing
  vTaskDelay(100);

  spi_demo_dump_config();
  i2c_demo_dump_config();
  uart_demo_dump_config();

  vTaskSuspend(NULL);
}
