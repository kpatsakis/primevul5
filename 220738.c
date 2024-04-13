void vnc_display_add_client(const char *id, int csock, bool skipauth)
{
    VncDisplay *vs = vnc_display_find(id);

    if (!vs) {
        return;
    }
    vnc_connect(vs, csock, skipauth, false);
}