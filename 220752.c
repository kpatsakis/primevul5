static void vnc_display_close(VncDisplay *vs)
{
    if (!vs)
        return;
    vs->enabled = false;
    vs->is_unix = false;
    if (vs->lsock != -1) {
        qemu_set_fd_handler(vs->lsock, NULL, NULL, NULL);
        close(vs->lsock);
        vs->lsock = -1;
    }
    vs->ws_enabled = false;
    if (vs->lwebsock != -1) {
        qemu_set_fd_handler(vs->lwebsock, NULL, NULL, NULL);
        close(vs->lwebsock);
        vs->lwebsock = -1;
    }
    vs->auth = VNC_AUTH_INVALID;
    vs->subauth = VNC_AUTH_INVALID;
    if (vs->tlscreds) {
        object_unparent(OBJECT(vs->tlscreds));
    }
    g_free(vs->tlsaclname);
    vs->tlsaclname = NULL;
}