#include <zephyr/kernel.h>
#include <dt-bindings/zmk/modifiers.h>
#include "modifiers.h"

/*
 * Four boxes for Cmd, Opt, Ctrl and Shift in the space the WPM gauge used.
 * A box is filled while its modifier (left or right) is active.
 */

#define ICON_SIZE 9
#define BOX_SIZE 15
#define BOX_GAP 2
#define BOX_X 1
#define BOX_Y (46 + BUFFER_OFFSET_MIDDLE)
#define ICON_INSET ((BOX_SIZE - ICON_SIZE) / 2)

/* One row per uint16_t, bit 8 is the leftmost pixel. */
static const uint16_t icon_cmd[ICON_SIZE] = {
    0b011000110, 0b100101001, 0b100101001, 0b011111110, 0b000101000,
    0b011111110, 0b100101001, 0b100101001, 0b011000110,
};
static const uint16_t icon_opt[ICON_SIZE] = {
    0b000000000, 0b000000000, 0b111001111, 0b000100000, 0b000010000,
    0b000001000, 0b000000111, 0b000000000, 0b000000000,
};
static const uint16_t icon_ctrl[ICON_SIZE] = {
    0b000000000, 0b000000000, 0b000010000, 0b000101000, 0b001000100,
    0b010000010, 0b100000001, 0b000000000, 0b000000000,
};
static const uint16_t icon_shift[ICON_SIZE] = {
    0b000010000, 0b000101000, 0b001000100, 0b010000010, 0b111000111,
    0b001000100, 0b001000100, 0b001000100, 0b001111100,
};

static void draw_icon(lv_obj_t *canvas, int32_t x, int32_t y, const uint16_t *icon,
                      lv_draw_rect_dsc_t *dsc) {
    for (int row = 0; row < ICON_SIZE; row++) {
        int col = 0;
        while (col < ICON_SIZE) {
            if (!(icon[row] & BIT(ICON_SIZE - 1 - col))) {
                col++;
                continue;
            }
            int start = col;
            while (col < ICON_SIZE && (icon[row] & BIT(ICON_SIZE - 1 - col))) {
                col++;
            }
            canvas_draw_rect(canvas, x + start, y + row, col - start, 1, dsc);
        }
    }
}

static void draw_box(lv_obj_t *canvas, int index, const uint16_t *icon, bool active) {
    lv_draw_rect_dsc_t fg;
    init_rect_dsc(&fg, LVGL_FOREGROUND);
    lv_draw_rect_dsc_t bg;
    init_rect_dsc(&bg, LVGL_BACKGROUND);

    int32_t x = BOX_X + index * (BOX_SIZE + BOX_GAP);

    canvas_draw_rect(canvas, x, BOX_Y, BOX_SIZE, BOX_SIZE, &fg);
    if (!active) {
        canvas_draw_rect(canvas, x + 1, BOX_Y + 1, BOX_SIZE - 2, BOX_SIZE - 2, &bg);
    }
    draw_icon(canvas, x + ICON_INSET, BOX_Y + ICON_INSET, icon, active ? &bg : &fg);
}

void draw_modifiers_status(lv_obj_t *canvas, const struct status_state *state) {
    uint8_t mods = state->modifiers;

    draw_box(canvas, 0, icon_cmd, mods & (MOD_LGUI | MOD_RGUI));
    draw_box(canvas, 1, icon_opt, mods & (MOD_LALT | MOD_RALT));
    draw_box(canvas, 2, icon_ctrl, mods & (MOD_LCTL | MOD_RCTL));
    draw_box(canvas, 3, icon_shift, mods & (MOD_LSFT | MOD_RSFT));
}
