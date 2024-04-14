#include "i2c_demo.h"
#include "spi_demo.h"
#include "uart_demo.h"

void app_main() {
  spi_demo_start();
  i2c_demo_start();
  uart_demo_start();
}
