static int vnc_update_client(VncState *vs, int has_dirty, bool sync)
{
    vs->has_dirty += has_dirty;
    if (vs->need_update && vs->csock != -1) {
        VncDisplay *vd = vs->vd;
        VncJob *job;
        int y;
        int height, width;
        int n = 0;

        if (vs->output.offset && !vs->audio_cap && !vs->force_update)
            /* kernel send buffers are full -> drop frames to throttle */
            return 0;

        if (!vs->has_dirty && !vs->audio_cap && !vs->force_update)
            return 0;

        /*
         * Send screen updates to the vnc client using the server
         * surface and server dirty map.  guest surface updates
         * happening in parallel don't disturb us, the next pass will
         * send them to the client.
         */
        job = vnc_job_new(vs);

        height = pixman_image_get_height(vd->server);
        width = pixman_image_get_width(vd->server);

        y = 0;
        for (;;) {
            int x, h;
            unsigned long x2;
            unsigned long offset = find_next_bit((unsigned long *) &vs->dirty,
                                                 height * VNC_DIRTY_BPL(vs),
                                                 y * VNC_DIRTY_BPL(vs));
            if (offset == height * VNC_DIRTY_BPL(vs)) {
                /* no more dirty bits */
                break;
            }
            y = offset / VNC_DIRTY_BPL(vs);
            x = offset % VNC_DIRTY_BPL(vs);
            x2 = find_next_zero_bit((unsigned long *) &vs->dirty[y],
                                    VNC_DIRTY_BPL(vs), x);
            bitmap_clear(vs->dirty[y], x, x2 - x);
            h = find_and_clear_dirty_height(vs, y, x, x2, height);
            x2 = MIN(x2, width / VNC_DIRTY_PIXELS_PER_BIT);
            if (x2 > x) {
                n += vnc_job_add_rect(job, x * VNC_DIRTY_PIXELS_PER_BIT, y,
                                      (x2 - x) * VNC_DIRTY_PIXELS_PER_BIT, h);
            }
            if (!x && x2 == width / VNC_DIRTY_PIXELS_PER_BIT) {
                y += h;
                if (y == height) {
                    break;
                }
            }
        }

        vnc_job_push(job);
        if (sync) {
            vnc_jobs_join(vs);
        }
        vs->force_update = 0;
        vs->has_dirty = 0;
        return n;
    }

    if (vs->csock == -1) {
        vnc_disconnect_finish(vs);
    } else if (sync) {
        vnc_jobs_join(vs);
    }

    return 0;
}