static inline int is_hwc_enabled(SM501State *state, int crt)
{
    uint32_t addr = crt ? state->dc_crt_hwc_addr : state->dc_panel_hwc_addr;
    return addr & SM501_HWC_EN;
}