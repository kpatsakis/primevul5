static int vnc_width(VncDisplay *vd)
{
    return MIN(VNC_MAX_WIDTH, ROUND_UP(surface_width(vd->ds),
                                       VNC_DIRTY_PIXELS_PER_BIT));
}