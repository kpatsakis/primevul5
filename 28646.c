static bool blit_is_unsafe(struct CirrusVGAState *s)
{
    /* should be the case, see cirrus_bitblt_start */
    assert(s->cirrus_blt_width > 0);
    assert(s->cirrus_blt_height > 0);

    if (blit_region_is_unsafe(s, s->cirrus_blt_dstpitch,
                              s->cirrus_blt_dstaddr & s->cirrus_addr_mask)) {
        return true;
    }
    if (blit_region_is_unsafe(s, s->cirrus_blt_srcpitch,
                              s->cirrus_blt_srcaddr & s->cirrus_addr_mask)) {
        return true;
    }

    return false;
}