dissect_tcpopt_echo(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_tree *field_tree;
    proto_item *item;
    proto_item *length_item;
    guint32 echo;
    int offset = 0;

    item = proto_tree_add_item(tree, proto_tcp_option_echo, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(item, ett_tcp_opt_echo);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                        offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), TCPOLEN_ECHO))
        return tvb_captured_length(tvb);

    proto_tree_add_item_ret_uint(field_tree, hf_tcp_option_echo, tvb,
                        offset + 2, 4, ENC_BIG_ENDIAN, &echo);

    proto_item_append_text(item, ": %u", echo);
    tcp_info_append_uint(pinfo, "ECHO", echo);

    return tvb_captured_length(tvb);
}