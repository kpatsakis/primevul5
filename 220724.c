static void vnc_update_server_surface(VncDisplay *vd)
{
    qemu_pixman_image_unref(vd->server);
    vd->server = NULL;

    if (QTAILQ_EMPTY(&vd->clients)) {
        return;
    }

    vd->server = pixman_image_create_bits(VNC_SERVER_FB_FORMAT,
                                          vnc_width(vd),
                                          vnc_height(vd),
                                          NULL, 0);
}