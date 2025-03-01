static void vnc_dpy_copy(DisplayChangeListener *dcl,
                         int src_x, int src_y,
                         int dst_x, int dst_y, int w, int h)
{
    VncDisplay *vd = container_of(dcl, VncDisplay, dcl);
    VncState *vs, *vn;
    uint8_t *src_row;
    uint8_t *dst_row;
    int i, x, y, pitch, inc, w_lim, s;
    int cmp_bytes;

    if (!vd->server) {
        /* no client connected */
        return;
    }

    vnc_refresh_server_surface(vd);
    QTAILQ_FOREACH_SAFE(vs, &vd->clients, next, vn) {
        if (vnc_has_feature(vs, VNC_FEATURE_COPYRECT)) {
            vs->force_update = 1;
            vnc_update_client(vs, 1, true);
            /* vs might be free()ed here */
        }
    }

    /* do bitblit op on the local surface too */
    pitch = vnc_server_fb_stride(vd);
    src_row = vnc_server_fb_ptr(vd, src_x, src_y);
    dst_row = vnc_server_fb_ptr(vd, dst_x, dst_y);
    y = dst_y;
    inc = 1;
    if (dst_y > src_y) {
        /* copy backwards */
        src_row += pitch * (h-1);
        dst_row += pitch * (h-1);
        pitch = -pitch;
        y = dst_y + h - 1;
        inc = -1;
    }
    w_lim = w - (VNC_DIRTY_PIXELS_PER_BIT - (dst_x % VNC_DIRTY_PIXELS_PER_BIT));
    if (w_lim < 0) {
        w_lim = w;
    } else {
        w_lim = w - (w_lim % VNC_DIRTY_PIXELS_PER_BIT);
    }
    for (i = 0; i < h; i++) {
        for (x = 0; x <= w_lim;
                x += s, src_row += cmp_bytes, dst_row += cmp_bytes) {
            if (x == w_lim) {
                if ((s = w - w_lim) == 0)
                    break;
            } else if (!x) {
                s = (VNC_DIRTY_PIXELS_PER_BIT -
                    (dst_x % VNC_DIRTY_PIXELS_PER_BIT));
                s = MIN(s, w_lim);
            } else {
                s = VNC_DIRTY_PIXELS_PER_BIT;
            }
            cmp_bytes = s * VNC_SERVER_FB_BYTES;
            if (memcmp(src_row, dst_row, cmp_bytes) == 0)
                continue;
            memmove(dst_row, src_row, cmp_bytes);
            QTAILQ_FOREACH(vs, &vd->clients, next) {
                if (!vnc_has_feature(vs, VNC_FEATURE_COPYRECT)) {
                    set_bit(((x + dst_x) / VNC_DIRTY_PIXELS_PER_BIT),
                            vs->dirty[y]);
                }
            }
        }
        src_row += pitch - w * VNC_SERVER_FB_BYTES;
        dst_row += pitch - w * VNC_SERVER_FB_BYTES;
        y += inc;
    }

    QTAILQ_FOREACH(vs, &vd->clients, next) {
        if (vnc_has_feature(vs, VNC_FEATURE_COPYRECT)) {
            vnc_copy(vs, src_x, src_y, dst_x, dst_y, w, h);
        }
    }
}