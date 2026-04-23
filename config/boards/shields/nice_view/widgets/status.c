/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include <math.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include "status.h"
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/ble_active_profile_changed.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/usb.h>
#include <zmk/ble.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>
#include <zmk/wpm.h>

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
#include <zmk/hid_indicators.h>
#include <zmk/events/hid_indicators_changed.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)
#include <zmk/split/central.h>
#endif

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct output_status_state {
    struct zmk_endpoint_instance selected_endpoint;
    int active_profile_index;
    bool active_profile_connected;
    bool active_profile_bonded;
};

struct layer_status_state {
    uint8_t index;
    const char *label;
};

struct wpm_status_state {
    uint8_t wpm;
};

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
struct indicator_status_state {
    bool caps_lock;
};
#endif

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)
struct peripheral_battery_status_state {
    uint8_t level;
    bool charging;
    bool connected;
};
#endif

// Draw dotted circle outline
static void draw_dotted_circle(lv_obj_t *canvas, int cx, int cy, int r, lv_color_t color,
                               int dots) {
    for (int i = 0; i < dots; i++) {
        float angle = 2.0f * 3.14159f * (float)i / (float)dots;
        int x = cx + (int)(r * cosf(angle));
        int y = cy + (int)(r * sinf(angle));
        if (x >= 0 && x < CANVAS_SIZE && y >= 0 && y < CANVAS_SIZE) {
            lv_canvas_set_px(canvas, x, y, color);
        }
    }
}

static void draw_top(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 0);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);

    // Fill background
    lv_canvas_draw_rect(canvas, 0, 0, CANVAS_SIZE, CANVAS_SIZE, &rect_black_dsc);

    // Battery row: left half = central, right half = peripheral. While
    // charging the voltage-based % is known-wrong, so show a centered bolt
    // glyph instead of the number. Right after unplug the cached number is
    // still inflated until the cell voltage relaxes; show two small centered
    // dots in that window. Numeric percent uses Montserrat 12.
    draw_batt_cell(canvas, 0,
                   state->charging, state->battery, state->battery_stale, true,
                   &rect_white_dsc);

    draw_batt_cell(canvas, 34,
                   state->peripheral_charging, state->peripheral_battery,
                   state->peripheral_battery_stale, state->peripheral_connected,
                   &rect_white_dsc);

    // Rotate canvas
    rotate_canvas(canvas, cbuf);
}

static void draw_middle(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 1);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);

    // Fill background
    lv_canvas_draw_rect(canvas, 0, 0, CANVAS_SIZE, CANVAS_SIZE, &rect_black_dsc);

    // Connection indicator: icon left-anchored in the left half, text
    // right-anchored in the right half. Fixed edges regardless of string
    // length.
    lv_draw_label_dsc_t icon_dsc;
    init_label_dsc(&icon_dsc, LVGL_FOREGROUND, &lv_font_montserrat_14, LV_TEXT_ALIGN_LEFT);
    lv_draw_label_dsc_t conn_label_dsc;
    init_label_dsc(&conn_label_dsc, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT);

    char conn_text[8] = {};
    const char *conn_icon = "";

    switch (state->selected_endpoint.transport) {
    case ZMK_TRANSPORT_USB:
        conn_icon = LV_SYMBOL_USB;
        strcat(conn_text, "USB");
        break;
    case ZMK_TRANSPORT_BLE:
        if (state->active_profile_bonded) {
            if (state->active_profile_connected) {
                conn_icon = LV_SYMBOL_WIFI;
                strcat(conn_text, "BT");
            } else {
                conn_icon = LV_SYMBOL_CLOSE;
                strcat(conn_text, "LOST");
            }
        } else {
            conn_icon = LV_SYMBOL_SETTINGS;
            strcat(conn_text, "OPEN");
        }
        break;
    }

    // Icon anchored to left of left half; text anchored to right of right half.
    lv_canvas_draw_text(canvas, 2, 7, 32, &icon_dsc, conn_icon);
    lv_canvas_draw_text(canvas, 34, 10, 32, &conn_label_dsc, conn_text);

    // Profile circles: diamond layout (3 top, 2 bottom centered)
    int r = 6;
    int row1_y = 36;
    int row2_y = 54;
    int profile_positions[5][2] = {
        {14, row1_y}, {34, row1_y}, {54, row1_y},
        {24, row2_y}, {44, row2_y},
    };

    for (int i = 0; i < 5; i++) {
        int cx = profile_positions[i][0];
        int cy = profile_positions[i][1];

        if (i == state->active_profile_index) {
            lv_draw_arc_dsc_t arc_filled;
            init_arc_dsc(&arc_filled, LVGL_FOREGROUND, r);
            lv_canvas_draw_arc(canvas, cx, cy, r, 0, 359, &arc_filled);
        } else {
            draw_dotted_circle(canvas, cx, cy, r, LVGL_FOREGROUND, 20);
        }
    }

    // Rotate canvas
    rotate_canvas(canvas, cbuf);
}

static void draw_bottom(lv_obj_t *widget, lv_color_t cbuf[], const struct status_state *state) {
    lv_obj_t *canvas = lv_obj_get_child(widget, 2);

    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);

    // Fill background
    lv_canvas_draw_rect(canvas, 0, 0, CANVAS_SIZE, CANVAS_SIZE, &rect_black_dsc);

    // WPM graph box
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);
    lv_draw_line_dsc_t line_dsc;
    init_line_dsc(&line_dsc, LVGL_FOREGROUND, 1);
    lv_draw_label_dsc_t label_dsc_wpm;
    init_label_dsc(&label_dsc_wpm, LVGL_FOREGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_RIGHT);

    lv_canvas_draw_rect(canvas, 0, 0, 68, 34, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 1, 1, 66, 32, &rect_black_dsc);

    char wpm_text[6] = {};
    snprintf(wpm_text, sizeof(wpm_text), "%d", state->wpm[9]);
    lv_canvas_draw_text(canvas, 42, 23, 24, &label_dsc_wpm, wpm_text);

    int max = 0;
    int min = 256;
    for (int i = 0; i < 10; i++) {
        if (state->wpm[i] > max) {
            max = state->wpm[i];
        }
        if (state->wpm[i] < min) {
            min = state->wpm[i];
        }
    }
    int range = max - min;
    if (range == 0) {
        range = 1;
    }
    lv_point_t points[10];
    for (int i = 0; i < 10; i++) {
        points[i].x = 2 + i * 7;
        points[i].y = 31 - (state->wpm[i] - min) * 28 / range;
    }
    lv_canvas_draw_line(canvas, points, 10, &line_dsc);

    // Layer name (moved up 4 rows to open a gap before the caps banner)
    lv_draw_label_dsc_t layer_dsc;
    init_label_dsc(&layer_dsc, LVGL_FOREGROUND, &lv_font_montserrat_14, LV_TEXT_ALIGN_CENTER);

    if (state->layer_label == NULL) {
        char text[10] = {};
        sprintf(text, "LAYER %i", state->layer_index);
        lv_canvas_draw_text(canvas, 0, 36, 68, &layer_dsc, text);
    } else {
        lv_canvas_draw_text(canvas, 0, 36, 68, &layer_dsc, state->layer_label);
    }

    // Caps lock indicator: full-width inverted bar with centered "CAPS" text.
    if (state->caps_lock) {
        lv_canvas_draw_rect(canvas, 0, 54, CANVAS_SIZE, 14, &rect_white_dsc);

        lv_draw_label_dsc_t caps_text_dsc;
        init_label_dsc(&caps_text_dsc, LVGL_BACKGROUND, &lv_font_unscii_8, LV_TEXT_ALIGN_CENTER);
        lv_canvas_draw_text(canvas, 0, 57, CANVAS_SIZE, &caps_text_dsc, "CAPS");
    }

    // Rotate canvas
    rotate_canvas(canvas, cbuf);
}

static void set_battery_status(struct zmk_widget_status *widget,
                               struct battery_status_state state) {
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool was_charging = widget->state.charging;
    widget->state.charging = state.usb_present;

    // On charging→not transition, mark the level stale: the previous sample
    // was taken under inflated charge voltage. The ZMK fork's relax handler
    // fires a fresh sample and forces an event ~7s later; this widget clears
    // stale on the next event after the unplug, regardless of whether the
    // level moved.
    if (was_charging && !state.usb_present) {
        widget->state.battery_stale = true;
    } else if (widget->state.battery_stale) {
        widget->state.battery_stale = false;
    }
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

    widget->state.battery = state.level;

    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_status(widget, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_status_state) {
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)
static void set_peripheral_battery_status(struct zmk_widget_status *widget,
                                          struct peripheral_battery_status_state state) {
    bool was_charging = widget->state.peripheral_charging;
    widget->state.peripheral_charging = state.charging;

    if (was_charging && !state.charging) {
        widget->state.peripheral_battery_stale = true;
    } else if (widget->state.peripheral_battery_stale) {
        widget->state.peripheral_battery_stale = false;
    }

    widget->state.peripheral_battery = state.level;
    widget->state.peripheral_connected = state.connected;
    draw_top(widget->obj, widget->cbuf, &widget->state);
}

static void peripheral_battery_status_update_cb(struct peripheral_battery_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_peripheral_battery_status(widget, state);
    }
}

static struct peripheral_battery_status_state
peripheral_battery_status_get_state(const zmk_event_t *eh) {
    uint8_t raw = 0;
    int ret = zmk_split_central_get_peripheral_battery_level(0, &raw);
    uint8_t level = raw & 0x7F;

    return (struct peripheral_battery_status_state){
        .level = level,
        .charging = (raw & 0x80) != 0,
        .connected = (ret == 0 && level > 0),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_battery_status,
                            struct peripheral_battery_status_state,
                            peripheral_battery_status_update_cb,
                            peripheral_battery_status_get_state)
ZMK_SUBSCRIPTION(widget_peripheral_battery_status, zmk_peripheral_battery_state_changed);
#endif /* CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING */

static void set_output_status(struct zmk_widget_status *widget,
                              const struct output_status_state *state) {
    widget->state.selected_endpoint = state->selected_endpoint;
    widget->state.active_profile_index = state->active_profile_index;
    widget->state.active_profile_connected = state->active_profile_connected;
    widget->state.active_profile_bonded = state->active_profile_bonded;

    draw_top(widget->obj, widget->cbuf, &widget->state);
    draw_middle(widget->obj, widget->cbuf2, &widget->state);
}

static void output_status_update_cb(struct output_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_output_status(widget, &state); }
}

static struct output_status_state output_status_get_state(const zmk_event_t *_eh) {
    return (struct output_status_state){
        .selected_endpoint = zmk_endpoints_selected(),
        .active_profile_index = zmk_ble_active_profile_index(),
        .active_profile_connected = zmk_ble_active_profile_is_connected(),
        .active_profile_bonded = !zmk_ble_active_profile_is_open(),
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_output_status, struct output_status_state,
                            output_status_update_cb, output_status_get_state)
ZMK_SUBSCRIPTION(widget_output_status, zmk_endpoint_changed);

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_output_status, zmk_usb_conn_state_changed);
#endif
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(widget_output_status, zmk_ble_active_profile_changed);
#endif

static void set_layer_status(struct zmk_widget_status *widget, struct layer_status_state state) {
    widget->state.layer_index = state.index;
    widget->state.layer_label = state.label;

    draw_bottom(widget->obj, widget->cbuf3, &widget->state);
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_layer_status(widget, state); }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    uint8_t index = zmk_keymap_highest_layer_active();
    return (struct layer_status_state){.index = index, .label = zmk_keymap_layer_name(index)};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

static void set_wpm_status(struct zmk_widget_status *widget, struct wpm_status_state state) {
    for (int i = 0; i < 9; i++) {
        widget->state.wpm[i] = widget->state.wpm[i + 1];
    }
    widget->state.wpm[9] = state.wpm;

    draw_bottom(widget->obj, widget->cbuf3, &widget->state);
}

static void wpm_status_update_cb(struct wpm_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_wpm_status(widget, state); }
}

struct wpm_status_state wpm_status_get_state(const zmk_event_t *eh) {
    return (struct wpm_status_state){.wpm = zmk_wpm_get_state()};
};

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state, wpm_status_update_cb,
                            wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
static void set_indicator_status(struct zmk_widget_status *widget,
                                 struct indicator_status_state state) {
    widget->state.caps_lock = state.caps_lock;
    draw_bottom(widget->obj, widget->cbuf3, &widget->state);
}

static void indicator_status_update_cb(struct indicator_status_state state) {
    struct zmk_widget_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_indicator_status(widget, state); }
}

static struct indicator_status_state indicator_status_get_state(const zmk_event_t *eh) {
    zmk_hid_indicators_t indicators = zmk_hid_indicators_get_current_profile();
    return (struct indicator_status_state){
        .caps_lock = (indicators & 0x02) != 0,
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_indicator_status, struct indicator_status_state,
                            indicator_status_update_cb, indicator_status_get_state)
ZMK_SUBSCRIPTION(widget_indicator_status, zmk_hid_indicators_changed);
#endif /* CONFIG_ZMK_HID_INDICATORS */

int zmk_widget_status_init(struct zmk_widget_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 160, 68);
    // Creation order sets LVGL z-order (later = on top). Middle overlaps top in
    // widget x=92..140 and needs to render over top's black background there, so
    // top must be created first.
    lv_obj_t *top = lv_canvas_create(widget->obj);
    lv_obj_align(top, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_canvas_set_buffer(top, widget->cbuf, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);
    lv_obj_t *middle = lv_canvas_create(widget->obj);
    lv_obj_align(middle, LV_ALIGN_TOP_LEFT, 72, 0);
    lv_canvas_set_buffer(middle, widget->cbuf2, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);
    lv_obj_t *bottom = lv_canvas_create(widget->obj);
    lv_obj_align(bottom, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_canvas_set_buffer(bottom, widget->cbuf3, CANVAS_SIZE, CANVAS_SIZE, LV_IMG_CF_TRUE_COLOR);

    sys_slist_append(&widgets, &widget->node);
    widget_battery_status_init();
#if IS_ENABLED(CONFIG_ZMK_SPLIT_BLE_CENTRAL_BATTERY_LEVEL_FETCHING)
    widget_peripheral_battery_status_init();
#endif
    widget_output_status_init();
    widget_layer_status_init();
    widget_wpm_status_init();
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
    widget_indicator_status_init();
#endif

    return 0;
}

lv_obj_t *zmk_widget_status_obj(struct zmk_widget_status *widget) { return widget->obj; }
