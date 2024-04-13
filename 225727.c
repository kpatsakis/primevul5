tcp_calculate_timestamps(packet_info *pinfo, struct tcp_analysis *tcpd,
            struct tcp_per_packet_data_t *tcppd)
{
    if( !tcppd ) {
        tcppd = wmem_new(wmem_file_scope(), struct tcp_per_packet_data_t);
        p_add_proto_data(wmem_file_scope(), pinfo, proto_tcp, pinfo->curr_layer_num, tcppd);
    }

    if (!tcpd)
        return;

    nstime_delta(&tcppd->ts_del, &pinfo->abs_ts, &tcpd->ts_prev);

    tcpd->ts_prev.secs=pinfo->abs_ts.secs;
    tcpd->ts_prev.nsecs=pinfo->abs_ts.nsecs;
}