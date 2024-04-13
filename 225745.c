tcp_sequence_number_analysis_print_lost(packet_info * pinfo,
                    proto_item * flags_item,
                    struct tcp_acked *ta
                    )
{
    /* TCP Lost Segment */
    if (ta->flags & TCP_A_LOST_PACKET) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_lost_packet);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                               "[TCP Previous segment not captured] ");
    }
    /* TCP Ack lost segment */
    if (ta->flags & TCP_A_ACK_LOST_PACKET) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_ack_lost_packet);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                               "[TCP ACKed unseen segment] ");
    }
}