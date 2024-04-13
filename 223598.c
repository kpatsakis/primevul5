static inline uint32_t get_hwc_y(SM501State *state, int crt)
{
    uint32_t location = crt ? state->dc_crt_hwc_location
                            : state->dc_panel_hwc_location;
    return (location & 0x07FF0000) >> 16;
}