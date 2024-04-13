dissect_tcpopt_md5(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_tree *field_tree;
    proto_item *item;
    proto_item *length_item;
    int offset = 0, optlen = tvb_reported_length(tvb);

    item = proto_tree_add_item(tree, proto_tcp_option_md5, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(item, ett_tcp_opt_md5);

    col_append_lstr(pinfo->cinfo, COL_INFO, " MD5", COL_ADD_LSTR_TERMINATOR);
    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                                      offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, optlen, TCPOLEN_MD5))
        return tvb_captured_length(tvb);

    proto_tree_add_item(field_tree, hf_tcp_option_md5_digest, tvb,
                        offset + 2, optlen - 2, ENC_NA);

    return tvb_captured_length(tvb);
}