tcp_sequence_number_analysis_print_keepalive(packet_info * pinfo,
                      proto_item * flags_item,
                      struct tcp_acked *ta
                      )
{
    /*TCP Keep Alive */
    if (ta->flags & TCP_A_KEEP_ALIVE) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_keep_alive);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP Keep-Alive] ");
    }
    /* TCP Ack Keep Alive */
    if (ta->flags & TCP_A_KEEP_ALIVE_ACK) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_keep_alive_ack);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP Keep-Alive ACK] ");
    }
}