init_tcp_conversation_data(packet_info *pinfo)
{
    struct tcp_analysis *tcpd;

    /* Initialize the tcp protocol data structure to add to the tcp conversation */
    tcpd=wmem_new0(wmem_file_scope(), struct tcp_analysis);
    tcpd->flow1.win_scale=-1;
    tcpd->flow1.window = G_MAXUINT32;
    tcpd->flow1.multisegment_pdus=wmem_tree_new(wmem_file_scope());

    tcpd->flow2.window = G_MAXUINT32;
    tcpd->flow2.win_scale=-1;
    tcpd->flow2.multisegment_pdus=wmem_tree_new(wmem_file_scope());

    /* Only allocate the data if its actually going to be analyzed */
    if (tcp_analyze_seq)
    {
        tcpd->flow1.tcp_analyze_seq_info = wmem_new0(wmem_file_scope(), struct tcp_analyze_seq_flow_info_t);
        tcpd->flow2.tcp_analyze_seq_info = wmem_new0(wmem_file_scope(), struct tcp_analyze_seq_flow_info_t);
    }
    /* Only allocate the data if its actually going to be displayed */
    if (tcp_display_process_info)
    {
        tcpd->flow1.process_info = wmem_new0(wmem_file_scope(), struct tcp_process_info_t);
        tcpd->flow2.process_info = wmem_new0(wmem_file_scope(), struct tcp_process_info_t);
    }

    tcpd->acked_table=wmem_tree_new(wmem_file_scope());
    tcpd->ts_first.secs=pinfo->abs_ts.secs;
    tcpd->ts_first.nsecs=pinfo->abs_ts.nsecs;
    nstime_set_zero(&tcpd->ts_mru_syn);
    nstime_set_zero(&tcpd->ts_first_rtt);
    tcpd->ts_prev.secs=pinfo->abs_ts.secs;
    tcpd->ts_prev.nsecs=pinfo->abs_ts.nsecs;
    tcpd->flow1.valid_bif = 1;
    tcpd->flow2.valid_bif = 1;
    tcpd->flow1.push_bytes_sent = 0;
    tcpd->flow2.push_bytes_sent = 0;
    tcpd->flow1.push_set_last = FALSE;
    tcpd->flow2.push_set_last = FALSE;
    tcpd->stream = tcp_stream_count++;
    tcpd->server_port = 0;

    return tcpd;
}