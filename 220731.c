static void vnc_init_basic_info_from_server_addr(int fd, VncBasicInfo *info,
                                                 Error **errp)
{
    struct sockaddr_storage sa;
    socklen_t salen;

    salen = sizeof(sa);
    if (getsockname(fd, (struct sockaddr*)&sa, &salen) < 0) {
        error_setg_errno(errp, errno, "getsockname failed");
        return;
    }

    vnc_init_basic_info(&sa, salen, info, errp);
}