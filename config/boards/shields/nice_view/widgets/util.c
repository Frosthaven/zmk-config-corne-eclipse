/*
 *
 * Copyright (c) 2023 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 */

#include <zephyr/kernel.h>
#include "util.h"

LV_IMG_DECLARE(bolt);

void rotate_canvas(lv_obj_t *canvas, lv_color_t cbuf[]) {
    static lv_color_t cbuf_tmp[CANVAS_SIZE * CANVAS_SIZE];
    memcpy(cbuf_tmp, cbuf, sizeof(cbuf_tmp));
    lv_img_dsc_t img;
    img.data = (void *)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CANVAS_SIZE;
    img.header.h = CANVAS_SIZE;

    lv_canvas_fill_bg(canvas, LVGL_BACKGROUND, LV_OPA_COVER);
    lv_canvas_transform(canvas, &img, 900, LV_IMG_ZOOM_NONE, -1, 0, CANVAS_SIZE / 2,
                        CANVAS_SIZE / 2, true);
}

void draw_battery(lv_obj_t *canvas, const struct status_state *state) {
    lv_draw_rect_dsc_t rect_black_dsc;
    init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
    lv_draw_rect_dsc_t rect_white_dsc;
    init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);

    lv_canvas_draw_rect(canvas, 0, 2, 29, 12, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 1, 3, 27, 10, &rect_black_dsc);
    lv_canvas_draw_rect(canvas, 2, 4, (state->battery + 2) / 4, 8, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 30, 5, 3, 6, &rect_white_dsc);
    lv_canvas_draw_rect(canvas, 31, 6, 1, 4, &rect_black_dsc);

    if (state->charging) {
        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        lv_canvas_draw_img(canvas, 9, -1, &bolt, &img_dsc);
    }
}

void init_label_dsc(lv_draw_label_dsc_t *label_dsc, lv_color_t color, const lv_font_t *font,
                    lv_text_align_t align) {
    lv_draw_label_dsc_init(label_dsc);
    label_dsc->color = color;
    label_dsc->font = font;
    label_dsc->align = align;
}

void init_rect_dsc(lv_draw_rect_dsc_t *rect_dsc, lv_color_t bg_color) {
    lv_draw_rect_dsc_init(rect_dsc);
    rect_dsc->bg_color = bg_color;
}

void init_line_dsc(lv_draw_line_dsc_t *line_dsc, lv_color_t color, uint8_t width) {
    lv_draw_line_dsc_init(line_dsc);
    line_dsc->color = color;
    line_dsc->width = width;
}

void init_arc_dsc(lv_draw_arc_dsc_t *arc_dsc, lv_color_t color, uint8_t width) {
    lv_draw_arc_dsc_init(arc_dsc);
    arc_dsc->color = color;
    arc_dsc->width = width;
}

void draw_batt_cell(lv_obj_t *canvas, int x, bool charging, uint8_t battery,
                    bool stale, bool connected, lv_draw_rect_dsc_t *rect_white) {
    if (!connected) {
        lv_draw_label_dsc_t dsc;
        init_label_dsc(&dsc, LVGL_FOREGROUND, &lv_font_montserrat_14, LV_TEXT_ALIGN_CENTER);
        lv_canvas_draw_text(canvas, x, 2, 34, &dsc, LV_SYMBOL_CLOSE);
        return;
    }
    if (charging) {
        lv_canvas_draw_rect(canvas, x, 0, 34, 20, rect_white);
        lv_draw_label_dsc_t bolt;
        init_label_dsc(&bolt, LVGL_BACKGROUND, &lv_font_montserrat_14, LV_TEXT_ALIGN_CENTER);
        lv_canvas_draw_text(canvas, x, 2, 34, &bolt, LV_SYMBOL_CHARGE);
        return;
    }
    if (stale) {
        /* Two 3x3 dots, vertically centered in the 20-row cell. Hand-drawn
         * rects so placement is pixel-exact regardless of font metrics. */
        lv_canvas_draw_rect(canvas, x + 13, 9, 3, 3, rect_white);
        lv_canvas_draw_rect(canvas, x + 19, 9, 3, 3, rect_white);
        return;
    }
    lv_draw_label_dsc_t label;
    init_label_dsc(&label, LVGL_FOREGROUND, &lv_font_montserrat_12, LV_TEXT_ALIGN_CENTER);
    char buf[4];
    snprintf(buf, sizeof(buf), "%d", battery);
    lv_canvas_draw_text(canvas, x, 4, 34, &label, buf);
}
