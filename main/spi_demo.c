#include "spi_demo.h"

#include <string.h>

#include "driver/spi_master.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SPI_CS_GPIO 9
#define SPI_CLK_GPIO 10
#define SPI_MISO_GPIO 11
#define SPI_MOSI_GPIO 12

#define SPI_CLOCK_MHZ 10
#define SPI_TASK_DELAY_MS 10

static char PAYLOAD[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
    0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40};

struct message {
  int16_t num;
  char bytes[sizeof(PAYLOAD)];
};

static spi_device_handle_t s_spi;

static void spi_demo_init() {
  spi_bus_config_t buscfg = {
      .mosi_io_num = SPI_MOSI_GPIO,
      .miso_io_num = SPI_MISO_GPIO,
      .sclk_io_num = SPI_CLK_GPIO,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = sizeof(struct message)};

  spi_device_interface_config_t devcfg = {
      .mode = 0,
      .clock_speed_hz = SPI_CLOCK_MHZ * 1000 * 1000,
      .spics_io_num = SPI_CS_GPIO,
      .flags = 0,
      .queue_size = 7,
      .pre_cb = NULL};

  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
  ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi));
}

static void spi_demo_task(void* arg) {
  struct message msg;
  msg.num = 0;
  memcpy(msg.bytes, PAYLOAD, sizeof(PAYLOAD));

  while (1) {
    msg.num++;

    spi_transaction_t t = {
        .length = 8 * sizeof(msg),
        .tx_buffer = &msg,
        .rx_buffer = NULL};

    ESP_ERROR_CHECK(spi_device_transmit(s_spi, &t));

    vTaskDelay(pdMS_TO_TICKS(SPI_TASK_DELAY_MS));
  }
}

void spi_demo_start() {
  spi_demo_init();
  xTaskCreate(spi_demo_task, "spi_task", 4096, NULL, 10, NULL);
}

void spi_demo_dump_config() {
  printf(
      "\nSPI\n"
      "  CS:        %d\n"
      "  CLK:       %d\n"
      "  MOSI:      %d\n"
      "  MISO:      %d\n"
      "  Clock:     %d MHz\n"
      "  Gap:       %d ms\n",
      SPI_CS_GPIO,
      SPI_CLK_GPIO,
      SPI_MOSI_GPIO,
      SPI_MISO_GPIO,
      SPI_CLOCK_MHZ,
      SPI_TASK_DELAY_MS);
}
