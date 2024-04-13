dissect_tcpopt_wscale(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    guint8 val;
    guint32 shift;
    proto_item *wscale_pi, *shift_pi, *gen_pi;
    proto_tree *wscale_tree;
    proto_item *length_item;
    int offset = 0;
    struct tcp_analysis *tcpd;

    tcpd=get_tcp_conversation_data(NULL,pinfo);

    wscale_pi = proto_tree_add_item(tree, proto_tcp_option_wscale, tvb, offset, -1, ENC_NA);
    wscale_tree = proto_item_add_subtree(wscale_pi, ett_tcp_option_wscale);

    proto_tree_add_item(wscale_tree, hf_tcp_option_kind, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    length_item = proto_tree_add_item(wscale_tree, hf_tcp_option_len, tvb, offset, 1, ENC_BIG_ENDIAN);
    offset += 1;

    if (!tcp_option_len_check(length_item, pinfo, tvb_reported_length(tvb), TCPOLEN_WINDOW))
        return tvb_captured_length(tvb);

    shift_pi = proto_tree_add_item_ret_uint(wscale_tree, hf_tcp_option_wscale_shift, tvb, offset, 1, ENC_BIG_ENDIAN, &shift);
    if (shift > 14) {
        /* RFC 1323: "If a Window Scale option is received with a shift.cnt
         * value exceeding 14, the TCP should log the error but use 14 instead
         * of the specified value." */
        shift = 14;
        expert_add_info(pinfo, shift_pi, &ei_tcp_option_wscale_shift_invalid);
    }

    gen_pi = proto_tree_add_uint(wscale_tree, hf_tcp_option_wscale_multiplier, tvb,
                                 offset, 1, 1 << shift);
    PROTO_ITEM_SET_GENERATED(gen_pi);
    val = tvb_get_guint8(tvb, offset);

    proto_item_append_text(wscale_pi, ": %u (multiply by %u)", val, 1 << shift);

    tcp_info_append_uint(pinfo, "WS", 1 << shift);

    if(!pinfo->fd->visited) {
        pdu_store_window_scale_option(shift, tcpd);
    }

    return tvb_captured_length(tvb);
}