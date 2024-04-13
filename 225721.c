dissect_tcpopt_user_to(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_item *tf;
    proto_tree *field_tree;
    proto_item *length_item;
    guint16 to;
    int offset = 0;

    tf = proto_tree_add_item(tree, proto_tcp_option_user_to, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(tf, ett_tcp_option_user_to);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                                      offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), TCPOLEN_USER_TO))
        return tvb_captured_length(tvb);

    proto_tree_add_item(field_tree, hf_tcp_option_user_to_granularity, tvb, offset + 2, 2, ENC_BIG_ENDIAN);
    to = tvb_get_ntohs(tvb, offset + 2) & 0x7FFF;
    proto_tree_add_item(field_tree, hf_tcp_option_user_to_val, tvb, offset + 2, 2, ENC_BIG_ENDIAN);

    tcp_info_append_uint(pinfo, "USER_TO", to);
    return tvb_captured_length(tvb);
}