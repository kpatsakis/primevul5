ssize_t vnc_client_write_buf(VncState *vs, const uint8_t *data, size_t datalen)
{
    ssize_t ret;
    int err = 0;
    if (vs->tls) {
        ret = qcrypto_tls_session_write(vs->tls, (const char *)data, datalen);
        if (ret < 0) {
            err = errno;
        }
    } else {
        ret = send(vs->csock, (const void *)data, datalen, 0);
        if (ret < 0) {
            err = socket_error();
        }
    }
    VNC_DEBUG("Wrote wire %p %zd -> %ld\n", data, datalen, ret);
    return vnc_client_io_error(vs, ret, err);
}