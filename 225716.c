mptcp_get_meta_from_token(struct tcp_analysis* tcpd, tcp_flow_t *tcp_flow, guint32 token) {

    struct mptcp_analysis* result = NULL;
    struct mptcp_analysis* mptcpd = tcpd->mptcp_analysis;
    guint8 assignedMetaId = 0;  /* array id < 2 */

    DISSECTOR_ASSERT(tcp_flow == tcpd->fwd || tcp_flow == tcpd->rev);



    /* if token already set for this meta */
    if( tcp_flow->mptcp_subflow->meta  && (tcp_flow->mptcp_subflow->meta->static_flags & MPTCP_META_HAS_TOKEN)) {
        return mptcpd;
    }

    /* else look for a registered meta with this token */
    result = (struct mptcp_analysis*)wmem_tree_lookup32(mptcp_tokens, token);

    /* if token already registered than just share it across TCP connections */
    if(result) {
        mptcpd = result;
        mptcp_attach_subflow(mptcpd, tcpd);
    }
    else {
        /* we create it if this connection */
        if(!mptcpd) {
            /* don't care which meta to choose assign each meta to a direction */
            mptcpd = mptcp_alloc_analysis(tcpd);
            mptcp_attach_subflow(mptcpd, tcpd);
        }
        else {

            /* already exists, thus some meta may already have been configured */
            if(mptcpd->meta_flow[0].static_flags & MPTCP_META_HAS_TOKEN) {
                assignedMetaId = 1;
            }
            else if(mptcpd->meta_flow[1].static_flags & MPTCP_META_HAS_TOKEN) {
                assignedMetaId = 0;
            }
            else {
                DISSECTOR_ASSERT_NOT_REACHED();
            }
            tcp_flow->mptcp_subflow->meta = &mptcpd->meta_flow[assignedMetaId];
        }
        DISSECTOR_ASSERT(tcp_flow->mptcp_subflow->meta);

        tcp_flow->mptcp_subflow->meta->token = token;
        tcp_flow->mptcp_subflow->meta->static_flags |= MPTCP_META_HAS_TOKEN;

        wmem_tree_insert32(mptcp_tokens, token, mptcpd);
    }

    DISSECTOR_ASSERT(mptcpd);


    /* compute the meta id assigned to tcp_flow */
    assignedMetaId = (tcp_flow->mptcp_subflow->meta == &mptcpd->meta_flow[0]) ? 0 : 1;

    /* computes the metaId tcpd->fwd should be assigned to */
    assignedMetaId = (tcp_flow == tcpd->fwd) ? assignedMetaId : (assignedMetaId +1) %2;

    tcpd->fwd->mptcp_subflow->meta = &mptcpd->meta_flow[ (assignedMetaId) ];
    tcpd->rev->mptcp_subflow->meta = &mptcpd->meta_flow[ (assignedMetaId +1) %2];

    return mptcpd;
}