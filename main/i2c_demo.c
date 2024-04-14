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

static char LOREM_IPSUM[] =
    "I2See lorem ipsum, start condition initiatus, SCL stretchum longus. "
    "Controller confusus, device addressus mismatchum. ACK expectedus, NACK "
    "receivedum.";

struct message {
  int32_t num;
  char txt[sizeof(LOREM_IPSUM)];
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
  memcpy(msg.txt, LOREM_IPSUM, sizeof(LOREM_IPSUM));

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
