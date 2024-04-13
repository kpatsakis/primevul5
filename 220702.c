VncInfo *qmp_query_vnc(Error **errp)
{
    VncInfo *info = g_malloc0(sizeof(*info));
    VncDisplay *vd = vnc_display_find(NULL);

    if (vd == NULL || !vd->enabled) {
        info->enabled = false;
    } else {
        struct sockaddr_storage sa;
        socklen_t salen = sizeof(sa);
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        info->enabled = true;

        /* for compatibility with the original command */
        info->has_clients = true;
        info->clients = qmp_query_client_list(vd);

        if (vd->lsock == -1) {
            return info;
        }

        if (getsockname(vd->lsock, (struct sockaddr *)&sa,
                        &salen) == -1) {
            error_setg(errp, QERR_UNDEFINED_ERROR);
            goto out_error;
        }

        if (getnameinfo((struct sockaddr *)&sa, salen,
                        host, sizeof(host),
                        serv, sizeof(serv),
                        NI_NUMERICHOST | NI_NUMERICSERV) < 0) {
            error_setg(errp, QERR_UNDEFINED_ERROR);
            goto out_error;
        }

        info->has_host = true;
        info->host = g_strdup(host);

        info->has_service = true;
        info->service = g_strdup(serv);

        info->has_family = true;
        info->family = inet_netfamily(sa.ss_family);

        info->has_auth = true;
        info->auth = g_strdup(vnc_auth_name(vd));
    }

    return info;

out_error:
    qapi_free_VncInfo(info);
    return NULL;
}