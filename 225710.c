dissect_tcpopt_correxp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    return dissect_tcpopt_default_option(tvb, pinfo, tree, proto_tcp_option_scpscor, ett_tcp_opt_scpscor);
}