mptcp_alloc_analysis(struct tcp_analysis* tcpd) {

    struct mptcp_analysis* mptcpd;

    DISSECTOR_ASSERT(tcpd->mptcp_analysis == 0);

    mptcpd = (struct mptcp_analysis*)wmem_new0(wmem_file_scope(), struct mptcp_analysis);
    mptcpd->subflows = wmem_list_new(wmem_file_scope());

    mptcpd->stream = mptcp_stream_count++;
    tcpd->mptcp_analysis = mptcpd;

    memset(&mptcpd->meta_flow, 0, 2*sizeof(mptcp_meta_flow_t));

    /* arbitrary assignment. Callers may override this */
    tcpd->fwd->mptcp_subflow->meta = &mptcpd->meta_flow[0];
    tcpd->rev->mptcp_subflow->meta = &mptcpd->meta_flow[1];

    return mptcpd;
}