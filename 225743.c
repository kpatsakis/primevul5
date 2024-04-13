dissect_tcpopt_default_option(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, int proto, int ett)
{
    proto_item *item;
    proto_tree *exp_tree;
    proto_item *length_item;
    int offset = 0;

    item = proto_tree_add_item(tree, proto, tvb, offset, -1, ENC_NA);
    exp_tree = proto_item_add_subtree(item, ett);

    proto_tree_add_item(exp_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(exp_tree, hf_tcp_option_len, tvb, offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), 2))
        return tvb_captured_length(tvb);

    return tvb_captured_length(tvb);
}