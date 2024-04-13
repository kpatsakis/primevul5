dissect_tcpopt_rvbd_trpy(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    proto_tree *field_tree;
    proto_item *pitem;
    proto_item *length_item;
    guint16 sport, dport, flags;
    int offset = 0,
        optlen = tvb_reported_length(tvb);
    static const int * rvbd_trpy_flags[] = {
        &hf_tcp_option_rvbd_trpy_flag_fw_rst_probe,
        &hf_tcp_option_rvbd_trpy_flag_fw_rst_inner,
        &hf_tcp_option_rvbd_trpy_flag_fw_rst,
        &hf_tcp_option_rvbd_trpy_flag_chksum,
        &hf_tcp_option_rvbd_trpy_flag_oob,
        &hf_tcp_option_rvbd_trpy_flag_mode,
        NULL
    };

    col_prepend_fstr(pinfo->cinfo, COL_INFO, "TRPY, ");

    pitem = proto_tree_add_item(tree, proto_tcp_option_rvbd_trpy, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(pitem, ett_tcp_opt_rvbd_trpy);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    length_item = proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                                      offset + 1, 1, ENC_BIG_ENDIAN);

    if (!tcp_option_len_check(length_item, pinfo, optlen, TCPOLEN_RVBD_TRPY_MIN))
        return tvb_captured_length(tvb);

    flags = tvb_get_ntohs(tvb, offset + TRPY_OPTIONS_OFFSET);
    proto_tree_add_bitmask_with_flags(field_tree, tvb, offset + TRPY_OPTIONS_OFFSET, hf_tcp_option_rvbd_trpy_flags,
                        ett_tcp_opt_rvbd_trpy_flags, rvbd_trpy_flags, ENC_NA, BMT_NO_APPEND);

    proto_tree_add_item(field_tree, hf_tcp_option_rvbd_trpy_src,
                        tvb, offset + TRPY_SRC_ADDR_OFFSET, 4, ENC_BIG_ENDIAN);

    proto_tree_add_item(field_tree, hf_tcp_option_rvbd_trpy_dst,
                        tvb, offset + TRPY_DST_ADDR_OFFSET, 4, ENC_BIG_ENDIAN);

    sport = tvb_get_ntohs(tvb, offset + TRPY_SRC_PORT_OFFSET);
    proto_tree_add_item(field_tree, hf_tcp_option_rvbd_trpy_src_port,
                        tvb, offset + TRPY_SRC_PORT_OFFSET, 2, ENC_BIG_ENDIAN);

    dport = tvb_get_ntohs(tvb, offset + TRPY_DST_PORT_OFFSET);
    proto_tree_add_item(field_tree, hf_tcp_option_rvbd_trpy_dst_port,
                        tvb, offset + TRPY_DST_PORT_OFFSET, 2, ENC_BIG_ENDIAN);

    proto_item_append_text(pitem, " %s:%u -> %s:%u",
                           tvb_ip_to_str(tvb, offset + TRPY_SRC_ADDR_OFFSET), sport,
                           tvb_ip_to_str(tvb, offset + TRPY_DST_ADDR_OFFSET), dport);

    /* Client port only set on SYN: optlen == 18 */
    if ((flags & RVBD_FLAGS_TRPY_OOB) && (optlen > TCPOLEN_RVBD_TRPY_MIN))
        proto_tree_add_item(field_tree, hf_tcp_option_rvbd_trpy_client_port,
                            tvb, offset + TRPY_CLIENT_PORT_OFFSET, 2, ENC_BIG_ENDIAN);

    /* Despite that we have the right TCP ports for other protocols,
     * the data is related to the Riverbed Optimization Protocol and
     * not understandable by normal protocol dissectors. If the sport
     * protocol is available then use that, otherwise just output it
     * as a hex-dump.
     */
    if (sport_handle != NULL) {
        conversation_t *conversation;
        conversation = find_or_create_conversation(pinfo);
        if (conversation_get_dissector(conversation, pinfo->num) != sport_handle) {
            conversation_set_dissector(conversation, sport_handle);
        }
    } else if (data_handle != NULL) {
        conversation_t *conversation;
        conversation = find_or_create_conversation(pinfo);
        if (conversation_get_dissector(conversation, pinfo->num) != data_handle) {
            conversation_set_dissector(conversation, data_handle);
        }
    }

    return tvb_captured_length(tvb);
}