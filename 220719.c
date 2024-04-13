void vnc_client_write(void *opaque)
{
    VncState *vs = opaque;

    vnc_lock_output(vs);
    if (vs->output.offset || vs->ws_output.offset) {
        vnc_client_write_locked(opaque);
    } else if (vs->csock != -1) {
        qemu_set_fd_handler(vs->csock, vnc_client_read, NULL, vs);
    }
    vnc_unlock_output(vs);
}