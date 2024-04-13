dissect_tcpopt_timestamp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_item *ti;
    proto_tree *ts_tree;
    proto_item *length_item;
    int offset = 0;
    guint32 ts_val, ts_ecr;
    int len = tvb_reported_length(tvb);

    ti = proto_tree_add_item(tree, proto_tcp_option_timestamp, tvb, offset, -1, ENC_NA);
    ts_tree = proto_item_add_subtree(ti, ett_tcp_option_timestamp);

    proto_tree_add_item(ts_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    length_item = proto_tree_add_item(ts_tree, hf_tcp_option_len, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    if (!tcp_option_len_check(length_item, pinfo, len, TCPOLEN_TIMESTAMP))
        return tvb_captured_length(tvb);

    proto_tree_add_item_ret_uint(ts_tree, hf_tcp_option_timestamp_tsval, tvb, offset,
                        4, ENC_BIG_ENDIAN, &ts_val);
    offset += 4;

    proto_tree_add_item_ret_uint(ts_tree, hf_tcp_option_timestamp_tsecr, tvb, offset,
                        4, ENC_BIG_ENDIAN, &ts_ecr);
    /* offset += 4; */

    proto_item_append_text(ti, ": TSval %u, TSecr %u", ts_val, ts_ecr);
    if (tcp_ignore_timestamps == FALSE) {
        tcp_info_append_uint(pinfo, "TSval", ts_val);
        tcp_info_append_uint(pinfo, "TSecr", ts_ecr);
    }

    return tvb_captured_length(tvb);
}