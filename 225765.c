gchar* tcp_follow_conv_filter(packet_info* pinfo, int* stream)
{
    conversation_t *conv;
    struct tcp_analysis *tcpd;

    if (((pinfo->net_src.type == AT_IPv4 && pinfo->net_dst.type == AT_IPv4) ||
        (pinfo->net_src.type == AT_IPv6 && pinfo->net_dst.type == AT_IPv6))
        && (conv=find_conversation_pinfo(pinfo, 0)) != NULL )
    {
        /* TCP over IPv4/6 */
        tcpd=get_tcp_conversation_data(conv, pinfo);
        if (tcpd == NULL)
            return NULL;

        *stream = tcpd->stream;
        return g_strdup_printf("tcp.stream eq %d", tcpd->stream);
    }

    return NULL;
}