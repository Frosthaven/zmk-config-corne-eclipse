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
                   state->central_battery_stale, central_cell_connected,
                   &rect_white_dsc);
#else
    /* Without the mirror feature, the central cell can't show real data;
     * treat it as always-disconnected so the row still renders cleanly. */
    draw_batt_cell(canvas, 0, false, 0, false, false, &rect_white_dsc);
#endif

    /* Peripheral (own) cell. */
    draw_batt_cell(canvas, 34, state->charging, state->battery, state->battery_stale,
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

int zmk_widget_status_init(struct zmk_widget_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 160, 68);
    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, widget->cbuf, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);

    lv_obj_t *art = lv_img_create(widget->obj);
    lv_img_set_src(art, &logo);
    lv_obj_align(art, LV_ALIGN_TOP_LEFT, 0, 0);

    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();
    widget_peripheral_status_init();
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_MIRROR)
    widget_central_battery_init();
#endif

    return 0;
}

lv_obj_t *zmk_widget_status_obj(struct zmk_widget_status *widget) { return widget->obj; }
