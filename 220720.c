ssize_t vnc_tls_push(const char *buf, size_t len, void *opaque)
{
    VncState *vs = opaque;
    ssize_t ret;

 retry:
    ret = send(vs->csock, buf, len, 0);
    if (ret < 0) {
        if (errno == EINTR) {
            goto retry;
        }
        return -1;
    }
    return ret;
}