#include "i2c_demo.h"

#include <string.h>

#include "driver/i2c.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define I2C_SDA_GPIO 4
#define I2C_SCL_GPIO 5
#define I2C_FREQ_HZ 100000
#define I2C_NUM I2C_NUM_0

#define I2C_ADDR 0x28

#define I2C_DEVICE_SDA_GPIO 6
#define I2C_DEVICE_SCL_GPIO 7
#define I2C_DEVICE_NUM I2C_NUM_1

#define I2C_TASK_DELAY_MS 10

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

static void i2c_controller_init() {
  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_SDA_GPIO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = I2C_SCL_GPIO,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_FREQ_HZ,
  };
  i2c_param_config(I2C_NUM, &conf);
  i2c_driver_install(I2C_NUM, conf.mode, 0, 0, 0);
}

static void i2c_device_init() {
  i2c_config_t conf = {
      .mode = I2C_MODE_SLAVE,
      .sda_io_num = I2C_DEVICE_SDA_GPIO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_io_num = I2C_DEVICE_SCL_GPIO,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .slave.addr_10bit_en = 0,
      .slave.slave_addr = I2C_ADDR};

  i2c_param_config(I2C_DEVICE_NUM, &conf);
  i2c_driver_install(I2C_DEVICE_NUM, conf.mode, 512, 512, 0);
}

static void i2c_controller_task(void* arg) {
  struct message msg;

  msg.num = 0;
  memcpy(msg.bytes, PAYLOAD, sizeof(PAYLOAD));

  while (1) {
    msg.num++;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (I2C_ADDR << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, (const uint8_t*) &msg, sizeof(msg), true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    vTaskDelay(pdMS_TO_TICKS(I2C_TASK_DELAY_MS));
  }
}

static void i2c_device_task(void* arg) {
  struct message msg;
  while (1) {
    i2c_slave_read_buffer(
        I2C_DEVICE_NUM,
        (uint8_t*) &msg,
        sizeof(msg),
        portMAX_DELAY);
  }
}

void i2c_demo_start() {
  i2c_controller_init();
  i2c_device_init();

  xTaskCreate(i2c_controller_task, "i2c_c_task", 4096, NULL, 10, NULL);
  xTaskCreate(i2c_device_task, "i2c_d_task", 4096, NULL, 10, NULL);
}

void i2c_demo_dump_config() {
  printf(
      "\nI2C\n"
      "  SDA:       %d\n"
      "  SCL:       %d\n"
      "  Address:   0x%02x\n"
      "  Clock:     %d Hz\n"
      "  Gap:       %d ms\n",
      I2C_SDA_GPIO,
      I2C_SCL_GPIO,
      I2C_ADDR,
      I2C_FREQ_HZ,
      I2C_TASK_DELAY_MS);
}
