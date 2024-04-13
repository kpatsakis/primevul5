static void vnc_set_area_dirty(DECLARE_BITMAP(dirty[VNC_MAX_HEIGHT],
                               VNC_MAX_WIDTH / VNC_DIRTY_PIXELS_PER_BIT),
                               VncDisplay *vd,
                               int x, int y, int w, int h)
{
    int width = vnc_width(vd);
    int height = vnc_height(vd);

    /* this is needed this to ensure we updated all affected
     * blocks if x % VNC_DIRTY_PIXELS_PER_BIT != 0 */
    w += (x % VNC_DIRTY_PIXELS_PER_BIT);
    x -= (x % VNC_DIRTY_PIXELS_PER_BIT);

    x = MIN(x, width);
    y = MIN(y, height);
    w = MIN(x + w, width) - x;
    h = MIN(y + h, height);

    for (; y < h; y++) {
        bitmap_set(dirty[y], x / VNC_DIRTY_PIXELS_PER_BIT,
                   DIV_ROUND_UP(w, VNC_DIRTY_PIXELS_PER_BIT));
    }
}