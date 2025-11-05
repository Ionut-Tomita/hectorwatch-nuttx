/****************************************************************************
 * apps/examples/lab04si/lab04si.c
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
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/boardctl.h>

#include <lvgl/lvgl.h>
#include <nuttx/timers/timer.h>
#include <nuttx/input/buttons.h>
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Should we perform board-specific driver initialization? There are two
 * ways that board initialization can occur:  1) automatically via
 * board_late_initialize() during bootupif CONFIG_BOARD_LATE_INITIALIZE
 * or 2).
 * via a call to boardctl() if the interface is enabled
 * (CONFIG_BOARDCTL=y).
 * If this task is running as an NSH built-in application, then that
 * initialization has probably already been performed otherwise we do it
 * here.
 */

#undef NEED_BOARDINIT

#if defined(CONFIG_BOARDCTL) && !defined(CONFIG_NSH_ARCHINIT)
  #define NEED_BOARDINIT 1
#endif

#define TIMER_DEVNAME "/dev/timer0"
/* Specifies how often the timer should generate events */
#define TIMER_INTERVAL 1000000
#define TIMER_SIGNO 32
#define BUTTONS_SIGNO 31
#define START_TIMER TCIOC_START
#define STOP_TIMER TCIOC_STOP

#define BUTTON_DEVNAME "/dev/buttons"
#define BUTTON_GPIO 0

/****************************************************************************
 * Private Type Declarations
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

static volatile int timer_fd;

/**
 * @TODO: (3) Define global variables to be used by the timer
 *
 */

/* ********************************************************************** */

/**
 * @TODO: (BONUS) Define global variables to be used by the button task
 *
 */

/* ********************************************************************** */

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void timer_sighandler(int signo, FAR siginfo_t *siginfo,
                             FAR void *context)
{
  /**
   * @TODO: (3) Fill the timer signal handler to update the value to be displayed
   *
   */

  /* ********************************************************************** */
}

static int setup_timer(void)
{
  struct timer_notify_s notify;
  struct sigaction act;
  int ret;

  timer_fd = open(TIMER_DEVNAME, O_RDONLY);
  if (timer_fd < 0) {
    fprintf(stderr, "ERROR: Failed to open %s: %d\n",
            TIMER_DEVNAME, errno);
    return EXIT_FAILURE;
  }

  ret = ioctl(timer_fd, TCIOC_SETTIMEOUT, TIMER_INTERVAL);
  if (ret < 0) {
    fprintf(stderr, "ERROR: Failed to set the timer interval: %d\n",
            errno);
    close(timer_fd);
    return EXIT_FAILURE;
  }

  act.sa_sigaction = timer_sighandler;
  act.sa_flags     = SA_SIGINFO;

  sigfillset(&act.sa_mask);
  sigdelset(&act.sa_mask, TIMER_SIGNO);

  ret = sigaction(TIMER_SIGNO, &act, NULL);
  if (ret != OK) {
    fprintf(stderr, "ERROR: Fsigaction failed: %d\n", errno);
    close(timer_fd);
    return EXIT_FAILURE;
  }

  notify.pid      = getpid();
  notify.periodic = true;

  notify.event.sigev_notify = SIGEV_SIGNAL;
  notify.event.sigev_signo  = TIMER_SIGNO;
  notify.event.sigev_value.sival_ptr = NULL;

  ret = ioctl(timer_fd, TCIOC_NOTIFICATION, (unsigned long)((uintptr_t)&notify));
  if (ret < 0) {
    fprintf(stderr, "ERROR: Failed to set the timer handler: %d\n", errno);
    close(timer_fd);
    return EXIT_FAILURE;
  }

  return 0;
}

static int send_timer_cmd(int cmd)
{
  int ret;
  struct timer_notify_s notify;

  if (cmd == STOP_TIMER) {

    ret = ioctl(timer_fd, cmd, 0);

    if (ret < 0) {
      printf("ERROR: Failed to send command to timer: %d\n", errno);
      close(timer_fd);
      return EXIT_FAILURE;
    }
  } else {

    notify.pid      = getpid();
    notify.periodic = true;

    notify.event.sigev_notify = SIGEV_SIGNAL;
    notify.event.sigev_signo  = TIMER_SIGNO;
    notify.event.sigev_value.sival_ptr = NULL;

    ret = ioctl(timer_fd, TCIOC_NOTIFICATION, (unsigned long)((uintptr_t)&notify));
    if (ret < 0) {
      printf("ERROR: Failed to set the timer handler: %d\n", errno);
      close(timer_fd);
      return EXIT_FAILURE;
    }

    ret = ioctl(timer_fd, cmd, 0);
    if (ret < 0) {
      printf("ERROR: Failed to send command to timer: %d\n", errno);
      close(timer_fd);
      return EXIT_FAILURE;
    }
  }

  return 0;
}

static int button_task(int argc, char *argv[])
{
  int ret;
  int fd;
  btn_buttonset_t supported;
  struct btn_notify_s btnevents;

  /* Open the BUTTON driver */

  printf("button_task: Opening %s\n", BUTTON_DEVNAME);
  fd = open(BUTTON_DEVNAME, O_RDONLY | O_NONBLOCK);
  if (fd < 0) {
    int errcode = errno;
    printf("button_task: ERROR: Failed to open %s: %d\n",
           BUTTON_DEVNAME, errcode);
    goto errout;
  }

  ret = ioctl(fd, BTNIOC_SUPPORTED,
              (unsigned long)((uintptr_t)&supported));
  if (ret < 0) {
    int errcode = errno;
    printf("button_task: ERROR: ioctl(BTNIOC_SUPPORTED) failed: %d\n",
           errcode);
    goto errout_with_fd;
  }

  printf("button_task: Supported BUTTONs 0x%02x\n",
         (unsigned int)supported);

  /* Handle only the buttton press event for the supported buttons */

  btnevents.bn_press   = supported;

  btnevents.bn_event.sigev_notify = SIGEV_SIGNAL;
  btnevents.bn_event.sigev_signo  = BUTTONS_SIGNO;

  /* Register to receive a signal when buttons are pressed/released */

  ret = ioctl(fd, BTNIOC_REGISTER,
              (unsigned long)((uintptr_t)&btnevents));
  if (ret < 0) {
    int errcode = errno;
    printf("button_task: ERROR: ioctl(BTNIOC_SUPPORTED) failed: %d\n",
           errcode);
    goto errout_with_fd;
  }

  /* Ignore the default signal action */

  signal(BUTTONS_SIGNO, SIG_IGN);

  while (1) {

    struct siginfo value;
    sigset_t set;

    /* Wait for a signal */
    sigemptyset(&set);
    sigaddset(&set, BUTTONS_SIGNO);
    ret = sigwaitinfo(&set, &value);
    if (ret < 0) {
      int errcode = errno;
      printf("button_task: ERROR: sigwaitinfo() failed: %d\n",
             errcode);
      goto errout_with_fd;
    }

    /**
     * @TODO: (BONUS) Add button event logic
     *
     */

    /* ********************************************************************** */

  }


errout_with_fd:
  close(fd);

errout:

  return EXIT_FAILURE;
}

static int lvgl_handler(int argc, char *argv[])
{
  while (1) {
    lv_timer_handler();
    usleep(20000);
  }

  return EXIT_FAILURE;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 *
 * Description:
 *
 * Input Parameters:
 *   Standard argc and argv
 *
 * Returned Value:
 *   Zero on success; a positive, non-zero value on failure.
 *
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int ret;
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;

#ifdef NEED_BOARDINIT
  /* Perform board-specific driver initialization */

  boardctl(BOARDIOC_INIT, 0);

#endif

  lv_init();

  lv_nuttx_dsc_init(&info);

#ifdef CONFIG_LV_USE_NUTTX_LCD
  info.fb_path = "/dev/lcd0";
#endif

  lv_nuttx_init(&info, &result);

  if (result.disp == NULL) {
    LV_LOG_ERROR("lv_demos initialization failure!");
    return 1;
  }

  /* Create a separate task for handling button events */
  ret = task_create("button_task", 110, 4096, button_task,
                    NULL);
  if (ret < 0) {
    int errcode = errno;
    printf("main: ERROR: Failed to start button_task: %d\n",
           errcode);
    return EXIT_FAILURE;
  }

  setup_timer();

  /* Change the active screen's background color */

  lv_obj_t *screen = lv_obj_create(NULL);
  lv_scr_load(screen);
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

  /* Create a white label, set its text and align it to the center */

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Timer:");
  lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  /**
   * @TODO: (2) Add a second label below the first one with a default timer value (00:00).
   *        Set the background color to RED.
   *
   */

  /* ********************************************************************** */

  /* Create a separate task for handling lvgl updates */
  ret = task_create("lvgl_handler", 110, 4096, lvgl_handler,
                    NULL);

  /**
   * @TODO: (3) Uncomment to start the timer
   *
   */

//   send_timer_cmd(START_TIMER);

  /* ********************************************************************** */

  while (1) {
    /**
     * @TODO: (3) Update the timer value on the display
     *
     */

    /* ********************************************************************** */

    /**
     * @TODO: (BONUS) On button press, start/stop the timer.
     *
     */

    /* ********************************************************************** */

    usleep(100000);
  }

  lv_disp_remove(result.disp);
  lv_deinit();

  return 0;
}
