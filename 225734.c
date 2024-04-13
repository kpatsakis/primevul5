get_or_create_mptcpd_from_key(struct tcp_analysis* tcpd, tcp_flow_t *fwd, guint64 key, guint8 hmac_algo _U_) {

    guint32 token = 0;
    guint64 expected_idsn= 0;
    struct mptcp_analysis* mptcpd = tcpd->mptcp_analysis;

    if(fwd->mptcp_subflow->meta && (fwd->mptcp_subflow->meta->static_flags & MPTCP_META_HAS_KEY)) {
        return mptcpd;
    }

    /* MPTCP only standardizes SHA1 for now. */
    mptcp_cryptodata_sha1(key, &token, &expected_idsn);

    mptcpd = mptcp_get_meta_from_token(tcpd, fwd, token);

    DISSECTOR_ASSERT(fwd->mptcp_subflow->meta);

    fwd->mptcp_subflow->meta->key = key;
    fwd->mptcp_subflow->meta->static_flags |= MPTCP_META_HAS_KEY;
    fwd->mptcp_subflow->meta->base_dsn = expected_idsn;
    return mptcpd;
}