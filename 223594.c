static inline int get_width(SM501State *s, int crt)
{
    int width = crt ? s->dc_crt_h_total : s->dc_panel_h_total;
    return (width & 0x00000FFF) + 1;
}