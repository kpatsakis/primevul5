dissect_tcpopt_mss(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_item *item;
    proto_tree *exp_tree;
    proto_item *length_item;
    int offset = 0;
    guint32 mss;

    item = proto_tree_add_item(tree, proto_tcp_option_mss, tvb, offset, -1, ENC_NA);
    exp_tree = proto_item_add_subtree(item, ett_tcp_option_mss);

    proto_tree_add_item(exp_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(exp_tree, hf_tcp_option_len, tvb, offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), TCPOLEN_MSS))
        return tvb_captured_length(tvb);

    proto_tree_add_item_ret_uint(exp_tree, hf_tcp_option_mss_val, tvb, offset + 2, 2, ENC_BIG_ENDIAN, &mss);
    proto_item_append_text(item, ": %u bytes", mss);
    tcp_info_append_uint(pinfo, "MSS", mss);

    return tvb_captured_length(tvb);
}