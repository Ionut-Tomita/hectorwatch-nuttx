/****************************************************************************
 * boards/xtensa/esp32s3/esp32s3-hacktorwatch/src/esp32s3_drv2605l.c
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include <debug.h>

#include <nuttx/arch.h>
#include <nuttx/input/drv2605l.h>
#include <nuttx/ioexpander/ioexpander.h>
#include <nuttx/i2c/i2c_master.h>

#include "esp32s3_gpio.h"
#include "esp32s3_i2c.h"
#include "hacktorwatch.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_drv2605l_initialize
 *
 * Description:
 *   Initialize and register the DRV2605L Haptic Driver
 *
 * Input Parameters:
 *   devno - The device number, used to build the device path as
 *           /dev/haptic0
 *   busno - The I2C bus number
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/

int board_drv2605l_initialize(int devno, int busno)
{
  FAR struct i2c_master_s *i2c;
  int ret;

  sninfo("Initializing DRV2605L!\n");

  /* Initialize DRV2605L */

  i2c = esp32s3_i2cbus_initialize(busno);
  if (i2c != NULL)
    {
      /* Then try to register the drv in one of the two I2C
       * available controllers.
       */

      up_mdelay(250);
      ret = drv2605l_register(devno, i2c, NULL);
      if (ret < 0)
        {
          ierr("ERROR: Error registering DRV2605L in I2C%d\n", busno);
        }
    }
  else
    {
      ret = -ENODEV;
    }

  return ret;
}
