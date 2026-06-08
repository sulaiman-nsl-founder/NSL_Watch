#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>


LV_FONT_DECLARE(montserrat_72);   // Important!

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(flip_watch);

/* UI objects */
static lv_obj_t *ampm_label;
static lv_obj_t *hour_label;
static lv_obj_t *minute_label;
static lv_obj_t *date_label;

/* Time state */
static int hour        = 13;
static int minute      = 13;
static int day_of_week = 1;
static int day         = 8;
static int month       = 6;
static int year        = 2026;

static const char *days[] = {
    "SUN","MON","TUE","WED","THU","FRI","SAT"
};

static const char *months[] = {
    "JAN","FEB","MAR","APR","MAY","JUN",
    "JUL","AUG","SEP","OCT","NOV","DEC"
};

static void update_watchface(void)
{
    char buf[8];
    char date_buf[32];
    bool pm = (hour >= 12);
    int  h12 = hour % 12;

    if (h12 == 0) h12 = 12;

    lv_label_set_text(ampm_label, pm ? "PM" : "AM");

    snprintf(buf, sizeof(buf), "%02d", h12);
    lv_label_set_text(hour_label, buf);

    snprintf(buf, sizeof(buf), "%02d", minute);
    lv_label_set_text(minute_label, buf);

    snprintf(date_buf, sizeof(date_buf), "%s %02d %s %04d",
             days[day_of_week], day, months[month - 1], year);
    lv_label_set_text(date_label, date_buf);
}

static void create_watchface(void)
{
    lv_obj_t *scr = lv_scr_act();

    /* Black background */
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    /*shadow*/
    lv_obj_t *ampm_shadow = lv_label_create(scr);
    lv_label_set_text(ampm_shadow, "AM");

    /* ── AM/PM ─────────────────────────────────────
       Small, top-left, muted white                  */
    ampm_label = lv_label_create(scr);
    lv_obj_set_style_text_color(ampm_label,
        lv_color_white(), 0);          /* muted */
    lv_obj_set_style_text_font(ampm_label,
        &lv_font_montserrat_24, 0);
    
    lv_obj_set_style_text_letter_space(
    ampm_label,
    2,
    0);
    lv_obj_align(ampm_label,
        LV_ALIGN_TOP_LEFT, 12, 10);

    // lv_obj_set_style_text_color(
    //     ampm_shadow,
    //     lv_color_white(),
    //     0);

    // lv_obj_set_style_text_font(
    //     ampm_shadow,
    //     &lv_font_montserrat_24,
    //     0);
    // lv_obj_align(ampm_shadow,
    //     LV_ALIGN_TOP_LEFT, 13, 10);
    /* ── Hour ──────────────────────────────────────
       Huge font, left-aligned, upper portion        */
    hour_label = lv_label_create(scr);
    lv_obj_set_style_text_color(hour_label,
        lv_color_white(), 0);
    lv_obj_set_style_text_font(hour_label,
        &montserrat_72, 0);
    lv_obj_align(hour_label,
        LV_ALIGN_TOP_LEFT, 18, 40);

    /* ── Minute ────────────────────────────────────
       Large font, left-aligned, below hour          */
    minute_label = lv_label_create(scr);
    lv_obj_set_style_text_color(minute_label,
        lv_color_white(), 0);
    lv_obj_set_style_text_font(minute_label,
        &montserrat_72, 0);
    lv_obj_align(minute_label,
        LV_ALIGN_TOP_LEFT, 95, 100);

    /* ── Date ──────────────────────────────────────
       Small, bottom-left, muted                     */
    date_label = lv_label_create(scr);
    lv_obj_set_style_text_color(date_label,
        lv_color_white(), 0);          /* muted */
    lv_obj_set_style_text_font(date_label,
        &lv_font_montserrat_20, 0);
    lv_obj_align(date_label,
        LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_align(date_label,
        LV_ALIGN_BOTTOM_MID, 3, 0);

    update_watchface();
}

static void minute_timer_cb(lv_timer_t *timer)
{
    ARG_UNUSED(timer);

    minute++;
    if (minute >= 60) {
        minute = 0;
        hour++;
        if (hour >= 24) hour = 0;
    }

    update_watchface();
    LOG_INF("Time %02d:%02d", hour, minute);
}

int main(void)
{
    const struct device *display_dev =
        DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display not ready");
        return 0;
    }

    create_watchface();
    lv_timer_handler();

    if (display_blanking_off(display_dev)) {
        LOG_ERR("Failed to enable display");
        return 0;
    }

    lv_timer_create(minute_timer_cb, 60000, NULL);

    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(100));
    }

    return 0;
}