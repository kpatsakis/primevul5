mptcp_select_subflow_from_meta(const struct tcp_analysis *tcpd, const mptcp_meta_flow_t *meta)
{
    /* select the tcp_flow with appropriate direction */
    if( tcpd->flow1.mptcp_subflow->meta == meta) {
        return tcpd->flow1.mptcp_subflow;
    }
    else {
        return tcpd->flow2.mptcp_subflow;
    }
}