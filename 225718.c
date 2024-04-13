mptcpip_conversation_packet(void *pct, packet_info *pinfo, epan_dissect_t *edt _U_, const void *vip)
{
    conv_hash_t *hash = (conv_hash_t*) pct;
    const struct tcp_analysis *tcpd=(const struct tcp_analysis *)vip;
    const mptcp_meta_flow_t *meta=(const mptcp_meta_flow_t *)tcpd->fwd->mptcp_subflow->meta;

    add_conversation_table_data_with_conv_id(hash, &meta->ip_src, &meta->ip_dst,
        meta->sport, meta->dport, (conv_id_t) tcpd->mptcp_analysis->stream, 1, pinfo->fd->pkt_len,
                                              &pinfo->rel_ts, &pinfo->abs_ts, &tcp_ct_dissector_info, ENDPOINT_TCP);

    return TAP_PACKET_REDRAW;
}