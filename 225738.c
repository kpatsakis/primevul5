dissect_tcpopt_exp(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_item *item;
    proto_tree *exp_tree;
    guint16 magic;
    int offset = 0, optlen = tvb_reported_length(tvb);

    item = proto_tree_add_item(tree, proto_tcp_option_exp, tvb, offset, -1, ENC_NA);
    exp_tree = proto_item_add_subtree(item, ett_tcp_option_exp);

    proto_tree_add_item(exp_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(exp_tree, hf_tcp_option_len, tvb, offset + 1, 1, ENC_BIG_ENDIAN);
    if (tcp_exp_options_with_magic && ((optlen - 2) > 0)) {
        magic = tvb_get_ntohs(tvb, offset + 2);
        proto_tree_add_item(exp_tree, hf_tcp_option_exp_magic_number, tvb,
                            offset + 2, 2, ENC_BIG_ENDIAN);
        switch (magic) {
        case 0xf989:  /* RFC7413, TCP Fast Open */
            dissect_tcpopt_tfo_payload(tvb, offset+2, optlen-2, pinfo, exp_tree, data);
            break;
        default:
            /* Unknown magic number */
            break;
        }
    } else {
        proto_tree_add_item(exp_tree, hf_tcp_option_exp_data, tvb,
                            offset + 2, optlen - 2, ENC_NA);
        tcp_info_append_uint(pinfo, "Expxx", TRUE);
    }
    return tvb_captured_length(tvb);
}