#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

lv_obj_t * circle;
lv_obj_t * clock_label;
lv_obj_t * label_steps;
int total_steps = 0;
time_t now;

static void cb_night_mode_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        LV_UNUSED(obj);

        if (lv_obj_get_state(obj) & LV_STATE_CHECKED) {
            lv_color_t color = {.red = 128, .green = 128, .blue = 128};

            lv_obj_set_style_bg_color(circle, color, 0);
        } else {
            lv_obj_set_style_bg_color(circle, lv_color_white(), 0);
        }
    }
}

static void update_clock(lv_timer_t *timer)
{
    struct tm *tm_info;
    char buf[16];

    now += 1; /* Plus one second */
    tm_info = gmtime(&now);

    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);
    lv_label_set_text(clock_label, buf);
    lv_obj_invalidate(clock_label);
}

static void update_steps(lv_timer_t *timer)
{
    total_steps++;
    lv_label_set_text_fmt(label_steps, "Steps: %i", total_steps);
}

static void hacktorwatch_init_watch_face(void)
{
    lv_obj_t * cb_night_mode; /* Checkbox to enable night mode */
    lv_obj_t * steps_row; /* Put the steps label and the checkbox in the same row */

    /* The main object which will be the parent of all the following objects */
    circle = lv_obj_create(lv_screen_active());

    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0); 
    /* The size of the watch display is 240 x 240 */
    lv_obj_set_size(circle, 240, 240);
    lv_obj_align(circle, LV_ALIGN_CENTER, 0, 0);

    /* A label to show the current time */
    clock_label = lv_label_create(circle);
    lv_obj_align(clock_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_label_set_text(clock_label, "--:--:--");

    /* A checkbox that enables/disables the night mode */
    cb_night_mode = lv_checkbox_create(circle);
    lv_checkbox_set_text(cb_night_mode, "Night mode");
    lv_obj_add_state(cb_night_mode, 0);
    lv_obj_align(cb_night_mode, LV_ALIGN_LEFT_MID, 0, -30);
    lv_obj_add_event_cb(cb_night_mode, cb_night_mode_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_set_style_pad_left(cb_night_mode, 20, LV_PART_INDICATOR);
    lv_obj_set_style_pad_right(cb_night_mode, 20, LV_PART_INDICATOR);
    lv_obj_set_style_pad_top(cb_night_mode, 20, LV_PART_INDICATOR);
    lv_obj_set_style_pad_bottom(cb_night_mode, 20, LV_PART_INDICATOR);

    /*
     * A row which contains two elements: the refresh button and the
     * text which displays the number of steps.
     */
    steps_row = lv_obj_create(circle);
    lv_obj_set_flex_flow(steps_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_size(steps_row, lv_pct(90), lv_pct(30));
    lv_obj_align(steps_row, LV_ALIGN_BOTTOM_MID, 0, -30);

    lv_obj_t * btn_reset_steps = lv_button_create(steps_row);
    lv_obj_set_style_bg_image_src(btn_reset_steps, LV_SYMBOL_REFRESH, 0);
    lv_obj_align(btn_reset_steps, LV_ALIGN_BOTTOM_MID, 0, 0);

    label_steps = lv_label_create(steps_row);
    lv_label_set_text_fmt(label_steps, "Steps: %i", total_steps);
    lv_obj_align(label_steps, LV_ALIGN_CENTER, 0, 0);

    lv_obj_update_layout(circle);
}

int main()
{
    lv_nuttx_dsc_t info;
    lv_nuttx_result_t result;

    /* Initialize LVGL. */
    lv_init();
    lv_nuttx_dsc_init(&info);

    info.fb_path = "/dev/lcd0";
    info.input_path = "/dev/input0";
    lv_nuttx_init(&info, &result);

    if (result.disp == NULL) {
	    LV_LOG_ERROR("LVGL initialization failure!");
	    return 1;
    }

    /* Get the current time */
    now = time(NULL);

    hacktorwatch_init_watch_face();
    lv_timer_create(update_clock, 1000, NULL);
    lv_timer_create(update_steps, 500, NULL);

    while (1)
    {
        
	    lv_timer_handler();
	    usleep(1000);
    }

    return 0;
}