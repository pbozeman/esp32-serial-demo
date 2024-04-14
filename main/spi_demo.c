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

static char LOREM_IPSUM[] =
    "SPIsum dolor sit amet, MOSI connectus, MISO response non existent. "
    "Data shiftum leftorium. Ipsum clock stretchum.";

struct send_buffer {
  int32_t num;
  char txt[sizeof(LOREM_IPSUM)];
};

static spi_device_handle_t s_spi;

static void spi_demo_init() {
  spi_bus_config_t buscfg = {
      .mosi_io_num = SPI_MOSI_GPIO,
      .miso_io_num = SPI_MISO_GPIO,
      .sclk_io_num = SPI_CLK_GPIO,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = sizeof(struct send_buffer)};

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
  struct send_buffer buffer;
  buffer.num = 0;
  memcpy(buffer.txt, LOREM_IPSUM, sizeof(LOREM_IPSUM));

  while (1) {
    buffer.num++;

    spi_transaction_t t = {
        .length = 8 * sizeof(buffer),
        .tx_buffer = &buffer,
        .rx_buffer = NULL};

    ESP_ERROR_CHECK(spi_device_transmit(s_spi, &t));

    vTaskDelay(pdMS_TO_TICKS(SPI_TASK_DELAY_MS));
  }
}

void spi_demo_start() {
  spi_demo_init();
  xTaskCreate(spi_demo_task, "spi_task", 4096, NULL, 10, NULL);
}
