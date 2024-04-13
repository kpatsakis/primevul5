static inline void hwc_invalidate(SM501State *s, int crt)
{
    int w = get_width(s, crt);
    int h = get_height(s, crt);
    int bpp = get_bpp(s, crt);
    int start = get_hwc_y(s, crt);
    int end = MIN(h, start + SM501_HWC_HEIGHT) + 1;

    start *= w * bpp;
    end *= w * bpp;

    memory_region_set_dirty(&s->local_mem_region,
                            get_fb_addr(s, crt) + start, end - start);
}