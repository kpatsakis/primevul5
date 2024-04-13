mptcp_init_subflow(tcp_flow_t *flow)
{
    struct mptcp_subflow *sf = wmem_new0(wmem_file_scope(), struct mptcp_subflow);

    DISSECTOR_ASSERT(flow->mptcp_subflow == 0);
    flow->mptcp_subflow = sf;
    sf->ssn2dsn_mappings        = wmem_itree_new(wmem_file_scope());
    sf->dsn2packet_map         = wmem_itree_new(wmem_file_scope());
}