#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>

#include <lvgl.h>



#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(watchface);

static lv_obj_t *time_label;
static lv_obj_t *date_label;

static int hour = 12;
static int minute = 0;
static int second = 0;

static const char *days[] = {
    "SUN","MON","TUE","WED","THU","FRI","SAT"
};

static const char *months[] = {
    "JAN","FEB","MAR","APR","MAY","JUN",
    "JUL","AUG","SEP","OCT","NOV","DEC"
};

static int day_of_week = 5;
static int day = 5;
static int month = 6;
static int year = 2026;

static void update_watchface(void)
{
    char time_buf[16];
    char date_buf[32];

    snprintf(time_buf,
             sizeof(time_buf),
             "%02d:%02d",
             hour,
             minute);

    snprintf(date_buf,
             sizeof(date_buf),
             "%s %02d %s %04d",
             days[day_of_week],
             day,
             months[month - 1],
             year);

    lv_label_set_text(time_label, time_buf);
    lv_label_set_text(date_label, date_buf);
}

static void minute_timer_cb(lv_timer_t *timer)
{
    ARG_UNUSED(timer);

    minute++;

    if (minute >= 60) {
        minute = 0;
        hour++;

        if (hour >= 24) {
            hour = 0;
        }
    }

    update_watchface();

    LOG_INF("Updated %02d:%02d", hour, minute);
}

int main(void)
{
    const struct device *display_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display not ready");
        return 0;
    }

    lv_obj_t *scr = lv_scr_act();

    lv_obj_set_style_bg_color(
        scr,
        lv_color_white(),
        LV_PART_MAIN);

    lv_obj_set_style_bg_opa(
        scr,
        LV_OPA_COVER,
        LV_PART_MAIN);

    time_label = lv_label_create(scr);

    lv_obj_set_style_text_color(
        time_label,
        lv_color_black(),
        0);

    lv_obj_set_style_text_font(
        time_label,
        &lv_font_montserrat_16,
        0);

    lv_obj_align(
        time_label,
        LV_ALIGN_CENTER,
        0,
        -20);

    date_label = lv_label_create(scr);

    lv_obj_set_style_text_color(
        date_label,
        lv_color_black(),
        0);

    lv_obj_set_style_text_font(
        date_label,
        &lv_font_montserrat_12,
        0);

    lv_obj_align(
        date_label,
        LV_ALIGN_BOTTOM_MID,
        0,
        -10);

    update_watchface();

    lv_timer_create(
        minute_timer_cb,
        60000,
        NULL);

    display_blanking_off(display_dev);

    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(100));
    }

    return 0;
}