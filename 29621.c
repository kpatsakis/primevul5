static ssize_t stream_writev_cb(RedsStream *s, const struct iovec *iov, int iovcnt)
{
    ssize_t ret = 0;
    do {
        int tosend;
        ssize_t n, expected = 0;
        int i;
#ifdef IOV_MAX
        tosend = MIN(iovcnt, IOV_MAX);
#else
        tosend = iovcnt;
#endif
        for (i = 0; i < tosend; i++) {
            expected += iov[i].iov_len;
        }
        n = writev(s->socket, iov, tosend);
        if (n <= expected) {
            if (n > 0)
                ret += n;
            return ret == 0 ? n : ret;
        }
        ret += n;
        iov += tosend;
        iovcnt -= tosend;
    } while(iovcnt > 0);

    return ret;
}
