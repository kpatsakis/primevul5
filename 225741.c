tcp_print_sequence_number_analysis(packet_info *pinfo, tvbuff_t *tvb, proto_tree *parent_tree,
                          struct tcp_analysis *tcpd, guint32 seq, guint32 ack)
{
    struct tcp_acked *ta = NULL;
    proto_item *item;
    proto_tree *tree;
    proto_tree *flags_tree=NULL;

    if (!tcpd) {
        return;
    }
    if(!tcpd->ta) {
        tcp_analyze_get_acked_struct(pinfo->num, seq, ack, FALSE, tcpd);
    }
    ta=tcpd->ta;
    if(!ta) {
        return;
    }

    item=proto_tree_add_item(parent_tree, hf_tcp_analysis, tvb, 0, 0, ENC_NA);
    PROTO_ITEM_SET_GENERATED(item);
    tree=proto_item_add_subtree(item, ett_tcp_analysis);

    /* encapsulate all proto_tree_add_xxx in ifs so we only print what
       data we actually have */
    if(ta->frame_acked) {
        item = proto_tree_add_uint(tree, hf_tcp_analysis_acks_frame,
            tvb, 0, 0, ta->frame_acked);
            PROTO_ITEM_SET_GENERATED(item);

        /* only display RTT if we actually have something we are acking */
        if( ta->ts.secs || ta->ts.nsecs ) {
            item = proto_tree_add_time(tree, hf_tcp_analysis_ack_rtt,
            tvb, 0, 0, &ta->ts);
                PROTO_ITEM_SET_GENERATED(item);
        }
    }
    if (!nstime_is_zero(&tcpd->ts_first_rtt)) {
        item = proto_tree_add_time(tree, hf_tcp_analysis_first_rtt,
                tvb, 0, 0, &(tcpd->ts_first_rtt));
        PROTO_ITEM_SET_GENERATED(item);
    }

    if(ta->bytes_in_flight) {
        /* print results for amount of data in flight */
        tcp_sequence_number_analysis_print_bytes_in_flight(pinfo, tvb, tree, ta);
        tcp_sequence_number_analysis_print_push_bytes_sent(pinfo, tvb, tree, ta);
    }

    if(ta->flags) {
        item = proto_tree_add_item(tree, hf_tcp_analysis_flags, tvb, 0, 0, ENC_NA);
        PROTO_ITEM_SET_GENERATED(item);
        flags_tree=proto_item_add_subtree(item, ett_tcp_analysis);

        /* print results for reused tcp ports */
        tcp_sequence_number_analysis_print_reused(pinfo, item, ta);

        /* print results for retransmission and out-of-order segments */
        tcp_sequence_number_analysis_print_retransmission(pinfo, tvb, flags_tree, item, ta);

        /* print results for lost tcp segments */
        tcp_sequence_number_analysis_print_lost(pinfo, item, ta);

        /* print results for tcp window information */
        tcp_sequence_number_analysis_print_window(pinfo, item, ta);

        /* print results for tcp keep alive information */
        tcp_sequence_number_analysis_print_keepalive(pinfo, item, ta);

        /* print results for tcp duplicate acks */
        tcp_sequence_number_analysis_print_duplicate(pinfo, tvb, flags_tree, ta, tree);

        /* print results for tcp zero window  */
        tcp_sequence_number_analysis_print_zero_window(pinfo, item, ta);

    }

}