tcpip_conversation_packet(void *pct, packet_info *pinfo, epan_dissect_t *edt _U_, const void *vip)
{
    conv_hash_t *hash = (conv_hash_t*) pct;
    const struct tcpheader *tcphdr=(const struct tcpheader *)vip;

    add_conversation_table_data_with_conv_id(hash, &tcphdr->ip_src, &tcphdr->ip_dst, tcphdr->th_sport, tcphdr->th_dport, (conv_id_t) tcphdr->th_stream, 1, pinfo->fd->pkt_len,
                                              &pinfo->rel_ts, &pinfo->abs_ts, &tcp_ct_dissector_info, ENDPOINT_TCP);

    return TAP_PACKET_REDRAW;
}