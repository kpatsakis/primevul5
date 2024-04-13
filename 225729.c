tcp_sequence_number_analysis_print_reused(packet_info * pinfo,
                      proto_item * flags_item,
                      struct tcp_acked *ta
                      )
{
    /* TCP Ports Reused */
    if (ta->flags & TCP_A_REUSED_PORTS) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_reused_ports);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO,
                               "[TCP Port numbers reused] ");
    }
}