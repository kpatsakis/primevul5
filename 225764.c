dissect_tcpopt_tfo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_item *item;
    proto_tree *exp_tree;
    int offset = 0;

    item = proto_tree_add_item(tree, proto_tcp_option_tfo, tvb, offset, -1, ENC_NA);
    exp_tree = proto_item_add_subtree(item, ett_tcp_option_exp);
    proto_tree_add_item(exp_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(exp_tree, hf_tcp_option_len, tvb, offset + 1, 1, ENC_BIG_ENDIAN);

    dissect_tcpopt_tfo_payload(tvb, offset, tvb_reported_length(tvb), pinfo, exp_tree, data);
    return tvb_captured_length(tvb);
}