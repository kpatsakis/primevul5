dissect_tcpopt_qs(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_tree *field_tree;
    proto_item *item;
    proto_item *length_item;
    guint8 rate;
    int offset = 0;

    item = proto_tree_add_item(tree, proto_tcp_option_qs, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(item, ett_tcp_opt_qs);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                                      offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), TCPOLEN_QS))
        return tvb_captured_length(tvb);

    rate = tvb_get_guint8(tvb, offset + 2) & 0x0f;
    col_append_lstr(pinfo->cinfo, COL_INFO,
        " QSresp=", val_to_str_ext_const(rate, &qs_rate_vals_ext, "Unknown"),
        COL_ADD_LSTR_TERMINATOR);
    proto_tree_add_item(field_tree, hf_tcp_option_qs_rate, tvb,
                        offset + 2, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(field_tree, hf_tcp_option_qs_ttl_diff, tvb,
                        offset + 3, 1, ENC_BIG_ENDIAN);

    return tvb_captured_length(tvb);
}