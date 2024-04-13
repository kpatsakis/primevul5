static inline int get_height(SM501State *s, int crt)
{
    int height = crt ? s->dc_crt_v_total : s->dc_panel_v_total;
    return (height & 0x00000FFF) + 1;
}