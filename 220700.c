ssize_t vnc_client_io_error(VncState *vs, ssize_t ret, int last_errno)
{
    if (ret == 0 || ret == -1) {
        if (ret == -1) {
            switch (last_errno) {
                case EINTR:
                case EAGAIN:
#ifdef _WIN32
                case WSAEWOULDBLOCK:
#endif
                    return 0;
                default:
                    break;
            }
        }

        VNC_DEBUG("Closing down client sock: ret %zd, errno %d\n",
                  ret, ret < 0 ? last_errno : 0);
        vnc_disconnect_start(vs);

        return 0;
    }
    return ret;
}