static ram_addr_t get_fb_addr(SM501State *s, int crt)
{
    return (crt ? s->dc_crt_fb_addr : s->dc_panel_fb_addr) & 0x3FFFFF0;
}