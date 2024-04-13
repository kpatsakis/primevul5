tcp_sequence_number_analysis_print_window(packet_info * pinfo,
                      proto_item * flags_item,
                      struct tcp_acked *ta
                      )
{
    /* TCP Window Update */
    if (ta->flags & TCP_A_WINDOW_UPDATE) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_window_update);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP Window Update] ");
    }
    /* TCP Full Window */
    if (ta->flags & TCP_A_WINDOW_FULL) {
        expert_add_info(pinfo, flags_item, &ei_tcp_analysis_window_full);
        col_prepend_fence_fstr(pinfo->cinfo, COL_INFO, "[TCP Window Full] ");
    }
}