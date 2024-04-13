static inline void get_hwc_palette(SM501State *state, int crt, uint8_t *palette)
{
    int i;
    uint32_t color_reg;
    uint16_t rgb565;

    for (i = 0; i < 3; i++) {
        if (i + 1 == 3) {
            color_reg = crt ? state->dc_crt_hwc_color_3
                            : state->dc_panel_hwc_color_3;
        } else {
            color_reg = crt ? state->dc_crt_hwc_color_1_2
                            : state->dc_panel_hwc_color_1_2;
        }

        if (i + 1 == 2) {
            rgb565 = (color_reg >> 16) & 0xFFFF;
        } else {
            rgb565 = color_reg & 0xFFFF;
        }
        palette[i * 3 + 0] = ((rgb565 >> 11) * 527 + 23) >> 6; /* r */
        palette[i * 3 + 1] = (((rgb565 >> 5) & 0x3f) * 259 + 33) >> 6; /* g */
        palette[i * 3 + 2] = ((rgb565 & 0x1f) * 527 + 23) >> 6; /* b */
    }
}