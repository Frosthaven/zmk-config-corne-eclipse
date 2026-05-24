/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/ble.h>

#if IS_ENABLED(CONFIG_NRFX_POWER)
#include <hal/nrf_power.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)
#include <zmk/events/central_battery_state_changed.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_SPLIT_CENTRAL_STATUS_MIRROR)
#include <stdio.h>
#include <zmk/events/central_status_changed.h>
#endif

#include "peripheral_status.h"

LV_IMG_DECLARE(logo);

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct peripheral_status_state {
    bool connected;
};


static void draw_top(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 0);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);

    lv_canvas_draw_rect(canvas, 0, 0, CANVAS_SIZE, CANVAS_SIZE, &rect_black_dsc);

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)
    /* Central cell: mark "disconnected" if we haven't heard from the
     * central yet, or if the split link is currently down. */
    bool central_cell_connected = state->connected && state->central_battery_received;
    draw_batt_cell(canvas, 0, state->central_charging, state->central_battery,
                   state->central_battery_stale || state->central_battery_boot_stale,
                   central_cell_connected,
                   &rect_white_dsc);
#else
    /* Without the mirror feature, the central cell can't show real data;
     * treat it as always-disconnected so the row still renders cleanly. */
    draw_batt_cell(canvas, 0, false, 0, false, false, &rect_white_dsc);
#endif

    /* Peripheral (own) cell. */
    draw_batt_cell(canvas, 34, state->charging, state->battery,
                   state->battery_stale || state->battery_boot_stale,
                   true, &rect_white_dsc);

    rotate_canvas(canvas, cbuf);
}

static void set_battery_status(struct zmk_widget_status *widget,
                               struct battery_status_state state) {
    bool was_charging = widget->state.charging;
    widget->state.charging = state.usb_present;

    if (was_charging && !state.usb_present) {
        widget->state.battery_stale = true;
    } else if (widget->state.battery_stale) {
        widget->state.battery_stale = false;
    }

    widget->state.battery = state.level;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    return (struct battery_status_state) {
        .level = zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_NRFX_POWER)
        .usb_present = nrf_power_usbregstatus_vbusdet_get(NRF_POWER),
#else
        .usb_present = false,
#endif
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);

static struct peripheral_status_state get_state(const zmk_event_t *_eh) {
    return (struct peripheral_status_state){.connected = zmk_split_bt_peripheral_is_connected()};
}

static void set_connection_status(struct zmk_widget_status *widget,
                                  struct peripheral_status_state state) {
    widget->state.connected = state.connected;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void output_status_update_cb(struct peripheral_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_connection_status(widget, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status, struct peripheral_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_peripheral_status_changed);

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)

struct central_battery_mirror_state {
    uint8_t battery;
    bool charging;
};

static void set_central_battery_status(struct zmk_widget_status *widget,
                                       struct central_battery_mirror_state state) {
    bool was_charging = widget->state.central_charging;
    widget->state.central_charging = state.charging;

    /* Same stale-handling as the own cell: on charging→not, show ".."
     * until the next event with a relaxed reading arrives. */
    if (was_charging && !state.charging) {
        widget->state.central_battery_stale = true;
    } else if (widget->state.central_battery_stale) {
        widget->state.central_battery_stale = false;
    }

    widget->state.central_battery = state.battery;
    widget->state.central_battery_received = true;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void central_battery_update_cb(struct central_battery_mirror_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_central_battery_status(widget, state);
    }
}

static struct central_battery_mirror_state central_battery_get_state(const zmk_event_t *eh) {
    const struct zmk_central_battery_state_changed *ev = as_zmk_central_battery_state_changed(eh);
    return (struct central_battery_mirror_state){
        .battery  = ev ? ev->state_of_charge : 0,
        .charging = ev ? ev->usb_powered    : false,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_central_battery, struct central_battery_mirror_state,
                            central_battery_update_cb, central_battery_get_state)
ZMK_SUBSCRIPTION(widget_central_battery, zmk_central_battery_state_changed);

#endif // CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR

#if IS_ENABLED(CONFIG_ZMK_SPLIT_CENTRAL_STATUS_MIRROR)

/* Render the central's relocated status (left panel) when the central has a
 * trackpad and no display of its own. The data arrives over the split link
 * via zmk_central_status_changed. Drawn into the second canvas (child 1). */
static void draw_central_status(lv_obj_t *widget, lv_color_t cbuf[],
                                const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 1);
    if (!canvas) {
        return;
    }

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_canvas_draw_rect(canvas, 0, 0, CANVAS_SIZE, CANVAS_SIZE, &rect_black_dsc);

    lv_draw_label_dsc_t label_dsc;
    init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_montserrat_12, LV_TEXT_ALIGN_LEFT);

    char buf[16];

    if (!state->central_status_received) {
        lv_canvas_draw_text(canvas, 0, 2, CANVAS_SIZE, &label_dsc, "..");
        rotate_canvas(canvas, cbuf);
        return;
    }

    /* Layer (index; the central resolves names, the peripheral shows #). */
    snprintf(buf, sizeof(buf), "LYR %d", state->central_layer);
    lv_canvas_draw_text(canvas, 0, 2, CANVAS_SIZE, &label_dsc, buf);

    /* Endpoint + profile. */
    if (state->central_endpoint_usb) {
        snprintf(buf, sizeof(buf), "USB");
    } else {
        snprintf(buf, sizeof(buf), "BT%d%s", state->central_profile + 1,
                 state->central_active_connected ? "" : " x");
    }
    lv_canvas_draw_text(canvas, 0, 18, CANVAS_SIZE, &label_dsc, buf);

    /* WPM. */
    snprintf(buf, sizeof(buf), "WPM %d", state->central_wpm);
    lv_canvas_draw_text(canvas, 0, 34, CANVAS_SIZE, &label_dsc, buf);

    /* Caps lock. */
    if (state->central_caps_lock) {
        lv_canvas_draw_text(canvas, 0, 50, CANVAS_SIZE, &label_dsc, "CAPS");
    }

    rotate_canvas(canvas, cbuf);
}

struct central_status_mirror_state {
    uint8_t layer;
    uint8_t profile;
    uint8_t wpm;
    bool caps;
    bool active_connected;
    bool endpoint_usb;
};

static void set_central_status(struct zmk_widget_status *widget,
                               struct central_status_mirror_state state) {
    widget->state.central_layer = state.layer;
    widget->state.central_profile = state.profile;
    widget->state.central_wpm = state.wpm;
    widget->state.central_caps_lock = state.caps;
    widget->state.central_active_connected = state.active_connected;
    widget->state.central_endpoint_usb = state.endpoint_usb;
    widget->state.central_status_received = true;

    draw_central_status(widget->obj, widget->cbuf2, &widget->state);
}

static void central_status_update_cb(struct central_status_mirror_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_central_status(widget, state); }
}

static struct central_status_mirror_state central_status_get_state(const zmk_event_t *eh) {
    const struct zmk_central_status_changed *ev = as_zmk_central_status_changed(eh);
    return (struct central_status_mirror_state){
        .layer            = ev ? ev->layer : 0,
        .profile          = ev ? ev->profile : 0,
        .wpm              = ev ? ev->wpm : 0,
        .caps             = ev ? ev->caps_lock : false,
        .active_connected = ev ? ev->active_profile_connected : false,
        .endpoint_usb     = ev ? ev->endpoint_usb : false,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_central_status, struct central_status_mirror_state,
                            central_status_update_cb, central_status_get_state)
ZMK_SUBSCRIPTION(widget_central_status, zmk_central_status_changed);

#endif // CONFIG_ZMK_SPLIT_CENTRAL_STATUS_MIRROR

/* Cold-boot battery stale: hold ".." on the battery cells at widget
 * init and clear it after the ZMK fork's relax-poll (5s post-boot)
 * has had time to settle the loaded reading. Separate from the
 * existing post-unplug stale flag so the two don't interfere. */
static void clear_boot_stale_handler(struct k_work *work) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        widget->state.battery_boot_stale = false;
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)
        widget->state.central_battery_boot_stale = false;
#endif
        draw_top(widget->obj, widget->cbuf, &widget->state);
    }
}
static K_WORK_DELAYABLE_DEFINE(clear_boot_stale_work, clear_boot_stale_handler);

int zmk_widget_status_init(struct zmk_widget_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 160, 68);
    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, widget->cbuf, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);

#if IS_ENABLED(CONFIG_ZMK_SPLIT_CENTRAL_STATUS_MIRROR)
    /* Central has the trackpad and no display: this peripheral renders the
     * relocated central status in the left panel instead of the logo. */
    lv_obj_t *status_panel = lv_canvas_create(widget->obj);
    lv_obj_align(status_panel, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(status_panel, widget->cbuf2, CANVAS_SIZE, CANVAS_SIZE,
                         LV_IMG_CF_TRUE_COLOR);
#else
    lv_obj_t *art = lv_img_create(widget->obj);
    lv_img_set_src(art, &logo);
    lv_obj_align(art, LV_ALIGN_TOP_LEFT, 0, 0);
#endif

    /* Show ".." on the battery cells until the cold-boot relax-poll has
     * had time to land on a loaded reading. See clear_boot_stale_handler. */
    widget->state.battery_boot_stale = true;
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)
    widget->state.central_battery_boot_stale = true;
#endif
    k_work_schedule(&clear_boot_stale_work, K_SECONDS(6));

    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();
    widget_peripheral_status_init();
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)
    widget_central_battery_init();
#endif
#if IS_ENABLED(CONFIG_ZMK_SPLIT_CENTRAL_STATUS_MIRROR)
    widget->state.central_status_received = false;
    draw_central_status(widget->obj, widget->cbuf2, &widget->state);
    widget_central_status_init();
#endif

    return 0;
}

lv_obj_t *zmk_widget_status_obj(struct zmk_widget_status *widget) { return widget->obj; }
