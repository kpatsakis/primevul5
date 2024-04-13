static void vnc_init_basic_info(struct sockaddr_storage *sa,
                                socklen_t salen,
                                VncBasicInfo *info,
                                Error **errp)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];
    int err;

    if ((err = getnameinfo((struct sockaddr *)sa, salen,
                           host, sizeof(host),
                           serv, sizeof(serv),
                           NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
        error_setg(errp, "Cannot resolve address: %s",
                   gai_strerror(err));
        return;
    }

    info->host = g_strdup(host);
    info->service = g_strdup(serv);
    info->family = inet_netfamily(sa->ss_family);
}