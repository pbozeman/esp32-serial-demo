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

static char PAYLOAD[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
    0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40};

struct message {
  int16_t num;
  char txt[sizeof(PAYLOAD)];
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

  ESP_ERROR_CHECK(
      uart_driver_install(UART_NUM_1, UART_BUF_SIZE * 2, 0, 0, NULL, 0));
  ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(
      UART_NUM_1,
      UART_TX_GPIO,
      UART_RX_GPIO,
      UART_PIN_NO_CHANGE,
      UART_PIN_NO_CHANGE));
}

static void uart_demo_task(void* arg) {
  struct message buffer;
  buffer.num = 0;
  memcpy(buffer.txt, PAYLOAD, sizeof(PAYLOAD));

  while (1) {
    buffer.num++;
    uart_write_bytes(UART_NUM_1, (const char*) &buffer, sizeof(buffer));
    vTaskDelay(pdMS_TO_TICKS(UART_TASK_DELAY_MS));
  }
}

void uart_demo_start() {
  uart_demo_init();
  xTaskCreate(uart_demo_task, "uart_task", 4096, NULL, 10, NULL);
}
