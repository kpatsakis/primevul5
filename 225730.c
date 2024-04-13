dissect_tcpopt_recbound(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    return dissect_tcpopt_default_option(tvb, pinfo, tree, proto_tcp_option_scpsrec, ett_tcp_opt_recbound);
}