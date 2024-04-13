static void vnc_dpy_cursor_define(DisplayChangeListener *dcl,
                                  QEMUCursor *c)
{
    VncDisplay *vd = container_of(dcl, VncDisplay, dcl);
    VncState *vs;

    cursor_put(vd->cursor);
    g_free(vd->cursor_mask);

    vd->cursor = c;
    cursor_get(vd->cursor);
    vd->cursor_msize = cursor_get_mono_bpl(c) * c->height;
    vd->cursor_mask = g_malloc0(vd->cursor_msize);
    cursor_get_mono_mask(c, 0, vd->cursor_mask);

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        vnc_cursor_define(vs);
    }
}