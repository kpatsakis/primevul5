tcp_seq_analysis_packet( void *ptr, packet_info *pinfo, epan_dissect_t *edt _U_, const void *tcp_info)
{
    seq_analysis_info_t *sainfo = (seq_analysis_info_t *) ptr;
    const struct tcpheader *tcph = (const struct tcpheader *)tcp_info;
    const char* flags;
    seq_analysis_item_t *sai = sequence_analysis_create_sai_with_addresses(pinfo, sainfo);

    if (!sai)
        return TAP_PACKET_DONT_REDRAW;

    sai->frame_number = pinfo->num;

    sai->port_src=pinfo->srcport;
    sai->port_dst=pinfo->destport;

    flags = tcp_flags_to_str(NULL, tcph);

    if ((tcph->th_have_seglen)&&(tcph->th_seglen!=0)){
        sai->frame_label = g_strdup_printf("%s - Len: %u",flags, tcph->th_seglen);
    }
    else{
        sai->frame_label = g_strdup(flags);
    }

    wmem_free(NULL, (void*)flags);

    if (tcph->th_flags & TH_ACK)
        sai->comment = g_strdup_printf("Seq = %u Ack = %u",tcph->th_seq, tcph->th_ack);
    else
        sai->comment = g_strdup_printf("Seq = %u",tcph->th_seq);

    sai->line_style = 1;
    sai->conv_num = (guint16) tcph->th_stream;
    sai->display = TRUE;

    g_queue_push_tail(sainfo->items, sai);

    return TAP_PACKET_REDRAW;
}