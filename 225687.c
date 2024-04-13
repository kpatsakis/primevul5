mptcp_attach_subflow(struct mptcp_analysis* mptcpd, struct tcp_analysis* tcpd) {

    if(!wmem_list_find(mptcpd->subflows, tcpd)) {
        wmem_list_prepend(mptcpd->subflows, tcpd);
    }

    /* in case we merge 2 mptcp connections */
    tcpd->mptcp_analysis = mptcpd;
}