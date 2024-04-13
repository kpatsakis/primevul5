tcp_sequence_number_analysis_print_retransmission(packet_info * pinfo,
                          tvbuff_t * tvb,
                          proto_tree * flags_tree, proto_item * flags_item,
                          struct tcp_acked *ta
                          )
{
    /* TCP Retransmission */
    if (ta->flags & TCP_A_RETRANSMISSION) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_retransmission);

        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP Retransmission] ");

        if (ta->rto_ts.secs || ta->rto_ts.nsecs) {
            flags_item = proto_tree_add_time(flags_tree, hf_tcp_analysis_rto,
                                             tvb, 0, 0, &ta->rto_ts);
            PROTO_ITEM_SET_GENERATED(flags_item);
            flags_item=proto_tree_add_uint(flags_tree, hf_tcp_analysis_rto_frame,
                                           tvb, 0, 0, ta->rto_frame);
            PROTO_ITEM_SET_GENERATED(flags_item);
        }
    }
    /* TCP Fast Retransmission */
    if (ta->flags & TCP_A_FAST_RETRANSMISSION) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_fast_retransmission);
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_retransmission);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                               "[TCP Fast Retransmission] ");
    }
    /* TCP Spurious Retransmission */
    if (ta->flags & TCP_A_SPURIOUS_RETRANSMISSION) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_spurious_retransmission);
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_retransmission);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                               "[TCP Spurious Retransmission] ");
    }

    /* TCP Out-Of-Order */
    if (ta->flags & TCP_A_OUT_OF_ORDER) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_out_of_order);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP Out-Of-Order] ");
    }
}