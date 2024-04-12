#include <stdio.h>
#include <string.h>

#include "driver/spi_master.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PIN_NUM_CS 9
#define PIN_NUM_CLK 10
#define PIN_NUM_MISO 11
#define PIN_NUM_MOSI 12

#define SPI_CLOCK_MHZ 10

char LOREM_IPSUM[] =
    "Lorem ipsum dolor sit amet, qui minim labore adipisicing minim sint "
    "cillum sint consectetur cupidatat.";

struct send_buffer {
  int32_t num;
  char txt[sizeof(LOREM_IPSUM)];
};

void app_main() {
  spi_device_handle_t spi;

  spi_bus_config_t buscfg = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = PIN_NUM_MISO,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = sizeof(struct send_buffer)};

  spi_device_interface_config_t devcfg = {
      .mode = 0,
      .clock_speed_hz = SPI_CLOCK_MHZ * 1000 * 1000,
      .spics_io_num = PIN_NUM_CS,
      .flags = 0,
      .queue_size = 7,
      .pre_cb = NULL};

  ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
  ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));

  struct send_buffer buffer;
  buffer.num = 0;
  memcpy(buffer.txt, LOREM_IPSUM, sizeof(LOREM_IPSUM));

  while (1) {
    buffer.num++;

    spi_transaction_t t = {
        .length = 8 * sizeof(buffer),
        .tx_buffer = &buffer,
        .rx_buffer = NULL};

    ESP_ERROR_CHECK(spi_device_transmit(spi, &t));

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
