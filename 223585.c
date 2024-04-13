static inline int get_bpp(SM501State *s, int crt)
{
    int bpp = crt ? s->dc_crt_control : s->dc_panel_control;
    return 1 << (bpp & 3);
}