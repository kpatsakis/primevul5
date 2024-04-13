tcp_sequence_number_analysis_print_zero_window(packet_info * pinfo,
                          proto_item * flags_item,
                          struct tcp_acked *ta
                        )
{
    /* TCP Zero Window Probe */
    if (ta->flags & TCP_A_ZERO_WINDOW_PROBE) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_zero_window_probe);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP ZeroWindowProbe] ");
    }
    /* TCP Zero Window */
    if (ta->flags&TCP_A_ZERO_WINDOW) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_zero_window);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP ZeroWindow] ");
    }
    /* TCP Zero Window Probe Ack */
    if (ta->flags & TCP_A_ZERO_WINDOW_PROBE_ACK) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_zero_window_probe_ack);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                               "[TCP ZeroWindowProbeAck] ");
    }
}