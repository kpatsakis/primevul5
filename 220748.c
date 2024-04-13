static int vnc_height(VncDisplay *vd)
{
    return MIN(VNC_MAX_HEIGHT, surface_height(vd->ds));
}