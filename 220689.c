static VncClientInfoList *qmp_query_client_list(VncDisplay *vd)
{
    VncClientInfoList *cinfo, *prev = NULL;
    VncState *client;

    QTAILQ_FOREACH(client, &vd->clients, next) {
        cinfo = g_new0(VncClientInfoList, 1);
        cinfo->value = qmp_query_vnc_client(client);
        cinfo->next = prev;
        prev = cinfo;
    }
    return prev;
}