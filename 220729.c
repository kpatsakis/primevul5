static VncBasicInfoList *qmp_query_server_entry(int socket,
                                                bool websocket,
                                                VncBasicInfoList *prev)
{
    VncBasicInfoList *list;
    VncBasicInfo *info;
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    if (getsockname(socket, (struct sockaddr *)&sa, &salen) < 0 ||
        getnameinfo((struct sockaddr *)&sa, salen,
                    host, sizeof(host), serv, sizeof(serv),
                    NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
        return prev;
    }

    info = g_new0(VncBasicInfo, 1);
    info->host = g_strdup(host);
    info->service = g_strdup(serv);
    info->family = inet_netfamily(sa.ss_family);
    info->websocket = websocket;

    list = g_new0(VncBasicInfoList, 1);
    list->value = info;
    list->next = prev;
    return list;
}