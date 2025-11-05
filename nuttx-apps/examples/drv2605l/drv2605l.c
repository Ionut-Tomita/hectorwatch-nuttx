/****************************************************************************
 * apps/examples/drv2605l/drv2605l.c
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

#include <stdio.h>
#include <sys/ioctl.h>

#include <nuttx/fs/ioctl.h>
#include <nuttx/input/ff.h>
#include <nuttx/mqueue.h>

#define N_EFFECTS (15)
#define HAPTIC_DEVICE "/dev/input_ff0"

#define STRONG_CLICK_100 (1)
#define STRONG_CLICK_60 (2)
#define DOUBLE_CLICK_100 (10)
#define STRONG_BUZZ (14)
#define SHARP_TICK_1 (25)
#define SHORT_DOUBLE_CLICK_STRONG_1 (28)
#define PULSING_STRONG_100 (52)
#define TRANSITION_HUM_1 (64)

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

struct haptic_data_s {
  int fd;                        /* Force-feedback device fd */
  int n_effects;                 /* Number of uploaded effects */
  char *effect_names[N_EFFECTS]; /* Their position should correspond with their id */
};

/****************************************************************************
 * Private Types
 ****************************************************************************/

static struct haptic_data_s g_haptic_data = {
  .fd = -1,
  .n_effects = 0,
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void upload_constant_effect(char *name, int16_t level,
                                   uint16_t length, uint16_t delay)
{
  struct ff_effect effect = {0};

  if (g_haptic_data.fd < 0 || g_haptic_data.n_effects == N_EFFECTS) {
    return;
  }

  effect.type = FF_CONSTANT;
  effect.id = -1;
  effect.u.constant.level = level;
  effect.replay.length = length;
  effect.replay.delay = delay; /* in ms */

  /**
   *  The haptic driver saves the effect internally,
   *  so the memory used here can be discarded after.
   */
  if (ioctl(g_haptic_data.fd, EVIOCSFF, &effect) < 0)
    {
      perror("Error: ioctl failed");
      return;
    }

  g_haptic_data.effect_names[g_haptic_data.n_effects] = name;
  g_haptic_data.n_effects++;
}

static void upload_rom_effect(char *name, int16_t number, uint16_t delay)
{
  struct ff_effect effect = {0};

  if (g_haptic_data.fd < 0 || g_haptic_data.n_effects == N_EFFECTS) {
    return;
  }

  effect.type = FF_PERIODIC;
  effect.id = -1;
  effect.u.periodic.waveform = FF_CUSTOM;
  effect.u.periodic.custom_len = 1;
  effect.u.periodic.custom_data = &number;  /* Play effect with this number */
  effect.replay.delay = delay;  /* in ms */

  /**
   *  The haptic driver saves the effect internally,
   *  so the memory used here can be discarded after.
   */
  if (ioctl(g_haptic_data.fd, EVIOCSFF, &effect) < 0)
    {
      perror("Error: ioctl failed");
      return;
    }

  g_haptic_data.effect_names[g_haptic_data.n_effects] = name;
  g_haptic_data.n_effects++;
}

static void play_effect(int8_t number)
{
  struct ff_event_s play;

  if (number >= 0 && number < N_EFFECTS)
    {
      memset(&play, 0, sizeof(play));
      play.code = number;
      play.value = 1;
      if (write(g_haptic_data.fd, &play, sizeof(play)) < 0)
        {
          perror("Play effect failed");
        }
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

char **get_avail_effects(void)
{
  return g_haptic_data.effect_names;
}

int main(int argc, char *argv[])
{
  g_haptic_data.fd = open(HAPTIC_DEVICE, O_WRONLY);
  if (g_haptic_data.fd < 0)
    {
    perror("Open device file");
    return -errno;
    }

  /* Upload effects */
  upload_rom_effect("Strong Click 100%%", STRONG_CLICK_100, 10);
  upload_rom_effect("Strong Click 60%%", STRONG_CLICK_60, 10);
  upload_rom_effect("Double Click 100%%", DOUBLE_CLICK_100, 10);
  upload_constant_effect("Constant 50%%", 0x2000, 1000, 0);

  play_effect(0);
  sleep(2);
  play_effect(1);
  sleep(2);
  play_effect(2);
  sleep(2);
  play_effect(3);
  sleep(2);

  close(g_haptic_data.fd);
}

