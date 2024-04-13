dissect_tcpopt_unknown(tvbuff_t *tvb, packet_info *pinfo _U_, proto_tree *tree, void* data _U_)
{
    proto_item *item;
    proto_tree *exp_tree;
    int offset = 0, optlen = tvb_reported_length(tvb);

    item = proto_tree_add_item(tree, proto_tcp_option_unknown, tvb, offset, -1, ENC_NA);
    exp_tree = proto_item_add_subtree(item, ett_tcp_unknown_opt);

    proto_tree_add_item(exp_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(exp_tree, hf_tcp_option_len, tvb, offset + 1, 1, ENC_BIG_ENDIAN);
    if (optlen > 2)
        proto_tree_add_item(exp_tree, hf_tcp_option_unknown_payload, tvb, offset + 2, optlen - 2, ENC_NA);

    return tvb_captured_length(tvb);
}