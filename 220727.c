ssize_t vnc_client_read_buf(VncState *vs, uint8_t *data, size_t datalen)
{
    ssize_t ret;
    int err = -1;
    if (vs->tls) {
        ret = qcrypto_tls_session_read(vs->tls, (char *)data, datalen);
        if (ret < 0) {
            err = errno;
        }
    } else {
        ret = qemu_recv(vs->csock, data, datalen, 0);
        if (ret < 0) {
            err = socket_error();
        }
    }
    VNC_DEBUG("Read wire %p %zd -> %ld\n", data, datalen, ret);
    return vnc_client_io_error(vs, ret, err);
}