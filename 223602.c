static inline uint8_t *get_hwc_address(SM501State *state, int crt)
{
    uint32_t addr = crt ? state->dc_crt_hwc_addr : state->dc_panel_hwc_addr;
    return state->local_mem + (addr & 0x03FFFFF0);
}