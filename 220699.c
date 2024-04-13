static void framebuffer_update_request(VncState *vs, int incremental,
                                       int x, int y, int w, int h)
{
    vs->need_update = 1;

    if (incremental) {
        return;
    }

    vs->force_update = 1;
    vnc_set_area_dirty(vs->dirty, vs->vd, x, y, w, h);
}