#include "uart_demo.h"

#include <string.h>
#include "driver/uart.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_TX_GPIO 17
#define UART_RX_GPIO 16

#define UART_BUF_SIZE 1024
#define UART_BAUD_RATE 115200
#define UART_TASK_DELAY_MS 1000

static char LOREM_IPSUM[] =
    "UARTsum dolor sit amet, transmit connectus, receive response non existent. "
    "Data shiftum leftorium. Ipsum baud stretchorium.";

struct message {
  int32_t num;
  char txt[sizeof(LOREM_IPSUM)];
};

static void uart_demo_init() {
  uart_config_t uart_config = {
      .baud_rate = UART_BAUD_RATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB,
  };

  ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, UART_BUF_SIZE * 2, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, UART_TX_GPIO, UART_RX_GPIO, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

static void uart_demo_task(void* arg) {
  struct message buffer;
  buffer.num = 0;
  memcpy(buffer.txt, LOREM_IPSUM, sizeof(LOREM_IPSUM));

  while (1) {
    buffer.num++;
    uart_write_bytes(UART_NUM_1, (const char*)&buffer, sizeof(buffer));
    vTaskDelay(pdMS_TO_TICKS(UART_TASK_DELAY_MS));
  }
}

void uart_demo_start() {
  uart_demo_init();
  xTaskCreate(uart_demo_task, "uart_task", 4096, NULL, 10, NULL);
}
