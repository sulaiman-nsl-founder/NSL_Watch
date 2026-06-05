#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(epaper);

int main(void)
{
    // Get display device
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if (!device_is_ready(display_dev)) {
        LOG_ERR("Display device not ready!");
        return 0;
    }
    LOG_INF("Display device ready.");

    // Get the current active screen and set its background to white
    // This is also an LVGL-level "clear" operation to ensure the canvas is white
    lv_obj_t *scr = lv_scr_act();
    if (scr == NULL) {
        LOG_ERR("LVGL screen is not ready!");
        return 0;
    }

    lv_obj_set_style_bg_color(scr, lv_color_white(), LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_STATE_DEFAULT);

    // Remove screen padding and scrollbar
    lv_obj_set_style_pad_all(scr, 0, LV_STATE_DEFAULT);
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);


    // Get display width and height (for layout)
    lv_disp_t *disp = lv_disp_get_default();
    if (disp == NULL) {
        LOG_ERR("LVGL display is not ready!");
        return 0;
    }

    lv_coord_t width = lv_disp_get_hor_res(disp);
    lv_coord_t height = lv_disp_get_ver_res(disp);
    LOG_INF("Display width: %d, height: %d", width, height);


    lv_obj_t *border = lv_obj_create(scr);
    lv_obj_set_size(border, width - 20, height - 20);
    lv_obj_align(border, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_opa(border, LV_OPA_TRANSP, LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(border, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(border, 2, LV_STATE_DEFAULT);
    lv_obj_set_style_radius(border, 0, LV_STATE_DEFAULT);

    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "HELLO EPAPER");
    lv_obj_set_style_text_color(label, lv_color_black(), LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, LV_STATE_DEFAULT);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_timer_handler();
    if (display_blanking_off(display_dev)) {
        LOG_ERR("Failed to turn off display blanking!");
        return 0;
    }

    while (1) {
        lv_timer_handler();
        k_sleep(K_MSEC(5000)); // Keep ePaper updates slow.
    }
    return 0;
}
