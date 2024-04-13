dissect_tcpopt_cc(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_tree *field_tree;
    proto_item *item;
    proto_item *length_item;
    int offset = 0;
    guint32 cc;

    item = proto_tree_add_item(tree, proto_tcp_option_cc, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(item, ett_tcp_opt_cc);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                        offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), TCPOLEN_CC))
        return tvb_captured_length(tvb);

    proto_tree_add_item_ret_uint(field_tree, hf_tcp_option_cc, tvb,
                        offset + 2, 4, ENC_BIG_ENDIAN, &cc);

    tcp_info_append_uint(pinfo, "CC", cc);
    return tvb_captured_length(tvb);
}