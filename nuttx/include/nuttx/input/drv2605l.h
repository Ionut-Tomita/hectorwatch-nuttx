/****************************************************************************
 * include/nuttx/input/drv2605l.h
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

#ifndef __INCLUDE_NUTTX_INPUT_DRV2605L_H_
#define __INCLUDE_NUTTX_INPUT_DRV2605L_H_

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdio.h>
#include <stdbool.h>
#include <nuttx/i2c/i2c_master.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: drv2605l_register
 *
 * Description:
 *   drv2605l haptic driver initialize
 *
 * Input Parameters:
 *   devno  - ff device number
 *   master - i2c master param
 *   ioedev - io dev pin set
 *
 * Returned Value:
 *   Zero (OK) on success; a negated errno value on failure.
 *
 ****************************************************************************/
int drv2605l_register(int devno, FAR struct i2c_master_s *i2c,
                      FAR struct ioexpander_dev_s *ioedev);

#endif /* __INCLUDE_NUTTX_INPUT_DRV2605L_H_ */
