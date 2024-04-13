tcp_init(void)
{
    tcp_stream_count = 0;

    /* MPTCP init */
    mptcp_stream_count = 0;
    mptcp_tokens = wmem_tree_new(wmem_file_scope());
}