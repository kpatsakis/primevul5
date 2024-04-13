static void vnc_disconnect_start(VncState *vs)
{
    if (vs->csock == -1)
        return;
    vnc_set_share_mode(vs, VNC_SHARE_MODE_DISCONNECTED);
    qemu_set_fd_handler(vs->csock, NULL, NULL, NULL);
    closesocket(vs->csock);
    vs->csock = -1;
}