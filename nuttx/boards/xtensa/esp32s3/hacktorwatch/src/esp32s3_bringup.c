/****************************************************************************
 * /boards/xtensa/esp32s3/hacktorwatch/src/esp32s3_bringup.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <syslog.h>
#include <debug.h>
#include <stdio.h>

#include <errno.h>
#include <nuttx/fs/fs.h>
#include <nuttx/himem/himem.h>
#include <arch/board/board.h>

#ifdef CONFIG_ESP32S3_TIMER
#  include "esp32s3_board_tim.h"
#endif

#ifdef CONFIG_ESP32S3_WIFI
#  include "esp32s3_board_wlan.h"
#endif

#ifdef CONFIG_ESPRESSIF_WIFI_BT_COEXIST
#  include "esp32s3_wifi_adapter.h"
#endif

#ifdef CONFIG_ESP32S3_BLE
#  include "esp32s3_ble.h"
#endif

#ifdef CONFIG_ESP32S3_RT_TIMER
#  include "esp32s3_rt_timer.h"
#endif

#ifdef CONFIG_ESP32S3_I2C
#  include "esp32s3_i2c.h"
#endif

#ifdef CONFIG_ESPRESSIF_I2S
#  include "espressif/esp_i2s.h"
#endif

#ifdef CONFIG_WATCHDOG
#  include "esp32s3_board_wdt.h"
#endif

#ifdef CONFIG_INPUT_BUTTONS
#  include <nuttx/input/buttons.h>
#endif

#ifdef CONFIG_INPUT_CST816S
#define CST816S_DEVICE_ADDRESS 0x15
#  include <nuttx/input/cst816s.h>
#  include "esp32s3_gpio.h"

#ifndef CONFIG_ESP32S3_GPIO_IRQ
#  error "The CST816S driver requires ESP32S3_GPIO_IRQ in the config"
#endif
#endif

#ifdef CONFIG_SENSORS_LSM6DSL
#include <nuttx/sensors/lsm6dsl.h>
#endif

#ifdef CONFIG_ESP32S3_PARTITION_TABLE
#  include "esp32s3_partition.h"
#endif

#ifdef CONFIG_ESP32S3_SPI
#include "esp32s3_spi.h"
#include "esp32s3_board_spidev.h"
#endif

#ifdef CONFIG_VIDEO_FB
#  include <nuttx/video/fb.h>
#elif defined(CONFIG_LCD_DEV)
#include <nuttx/board.h>
#include <nuttx/lcd/lcd_dev.h>
#endif

#include "hacktorwatch.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: esp32s3_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=n && CONFIG_BOARDCTL=y :
 *     Called from the NSH library
 *
 ****************************************************************************/


int esp32s3_bringup(void)
{
  int ret;
#if defined(CONFIG_ESP32S3_SPI) && defined(CONFIG_SPI_DRIVER)
  #ifdef CONFIG_ESP32S3_SPI2
  ret = board_spidev_initialize(ESP32S3_SPI2);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to init spidev 2: %d\n", ret);
    }
  #endif

  #ifdef CONFIG_ESP32S3_SPI3
  ret = board_spidev_initialize(ESP32S3_SPI3);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to init spidev 3: %d\n", ret);
    }
  #endif
#endif

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = nx_mount(NULL, "/proc", "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to mount procfs at /proc: %d\n", ret);
    }
#endif

#ifdef CONFIG_ESP32S3_TIMER
  /* Configure general purpose timers */

  ret = board_tim_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize timers: %d\n", ret);
    }
#endif

#ifdef CONFIG_ESP32S3_SPIFLASH
  ret = board_spiflash_init();
  if (ret)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize SPI Flash\n");
    }
#endif

#ifdef CONFIG_ESP32S3_PARTITION_TABLE
  ret = esp32s3_partition_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize partition error=%d\n",
             ret);
    }
#endif

#ifdef CONFIG_ESP32S3_RT_TIMER
  ret = esp32s3_rt_timer_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize RT timer: %d\n", ret);
    }
#endif

#ifdef CONFIG_RTC_DRIVER
  /* Instantiate the ESP32-S3 RTC driver */

  ret = esp32s3_rtc_driverinit();
  if (ret < 0)
    {
      syslog(LOG_ERR,
             "ERROR: Failed to Instantiate the RTC driver: %d\n", ret);
    }
#endif

#ifdef CONFIG_WATCHDOG
  /* Configure watchdog timer */

  ret = board_wdt_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize watchdog timer: %d\n", ret);
    }
#endif

#ifdef CONFIG_I2C_DRIVER
  /* Configure I2C peripheral interfaces */

  ret = board_i2c_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize I2C driver: %d\n", ret);
    }
#endif

#ifdef CONFIG_ESP32S3_TWAI

  /* Initialize TWAI and register the TWAI driver. */

  ret = esp32s3_twai_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: esp32s3_twai_setup failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_INPUT_BUTTONS
  /* Register the BUTTON driver */

  ret = btn_lower_initialize("/dev/buttons");
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize button driver: %d\n", ret);
    }
#endif


#ifdef CONFIG_ESPRESSIF_WIRELESS

#ifdef CONFIG_ESPRESSIF_WIFI_BT_COEXIST
  ret = esp_wifi_bt_coexist_init();
  if (ret)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize Wi-Fi and BT coexist\n");
    }
#endif

#ifdef CONFIG_ESPRESSIF_BLE
  ret = esp32s3_ble_initialize();
  if (ret)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize BLE\n");
    }
#endif

#ifdef CONFIG_ESPRESSIF_WLAN
  ret = board_wlan_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize wlan subsystem=%d\n",
             ret);
    }
#endif

#endif

#if defined(CONFIG_DEV_GPIO) && !defined(CONFIG_GPIO_LOWER_HALF)
  ret = esp32s3_gpio_init();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize GPIO Driver: %d\n", ret);
    }
#endif

#ifdef CONFIG_ESP32S3_I2C0
  struct i2c_master_s *i2c0 = esp32s3_i2cbus_initialize(0);
  if (!i2c0)
    {
      _err("ERROR: Failed to get I2C%d interface\n", 0);
    }
#endif

#ifdef CONFIG_INPUT_CST816S
  /* Configure the interrupt */
  esp32s3_configgpio(CST816S_INT, INPUT_PULLUP);
  int irq = ESP32S3_PIN2IRQ(CST816S_INT);

  /* Register the CST816S touch driver */
  ret = cst816s_register("/dev/input0", i2c0, CST816S_DEVICE_ADDRESS, irq);
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize CST816S touch driver: %d\n", ret);
    }

  esp32s3_gpioirqenable(irq, FALLING);

#endif


#ifdef CONFIG_SENSORS_LSM6DSL
  ret = lsm6dsl_sensor_register("/dev/lsm6dsl0", i2c0, LSM6DSLACCEL_ADDR0);
  if (ret < 0)
    {
      syslog("ERROR: Failed to initialize LMS6DSL accelero-gyro driver %s\n",
            "/dev/lsm6dsl0");
      return -ENODEV;
    }

  syslog(LOG_ERR, "INFO: LMS6DSL sensor has been initialized successfully\n");
#endif

#ifdef CONFIG_VIDEO_FB
  ret = fb_register(0, 0);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize Frame Buffer Driver.\n");
    }
#elif defined(CONFIG_LCD)
  ret = board_lcd_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize LCD.\n");
    }
  ret = lcddev_register(0);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to register LCD.\n");
    }

#endif

#ifdef CONFIG_FF_DRV2605L
  syslog(LOG_ERR, "Registering driver\n");
  ret = board_drv2605l_initialize(0, 0);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to register DRV2605L.\n");
    }
#endif /* CONFIG_FF_DRV2605L */

  /* If we got here then perhaps not all initialization was successful, but
   * at least enough succeeded to bring-up NSH with perhaps reduced
   * capabilities.
   */

  UNUSED(ret);
  return OK;
}
