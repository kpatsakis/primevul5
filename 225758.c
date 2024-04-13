dissect_tcpopt_sack(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
    proto_tree *field_tree = NULL;
    proto_item *tf, *ti;
    guint32 leftedge, rightedge;
    struct tcp_analysis *tcpd=NULL;
    struct tcpheader *tcph = (struct tcpheader *)data;
    guint32 base_ack=0;
    guint  num_sack_ranges = 0;
    int offset = 0;
    int optlen = tvb_reported_length(tvb);

    if(tcp_analyze_seq && tcp_relative_seq) {
        /* find(or create if needed) the conversation for this tcp session */
        tcpd=get_tcp_conversation_data(NULL,pinfo);

        if (tcpd) {
            base_ack=tcpd->rev->base_seq;
        }
    }

    ti = proto_tree_add_item(tree, proto_tcp_option_sack, tvb, offset, -1, ENC_NA);
    field_tree = proto_item_add_subtree(ti, ett_tcp_option_sack);

    proto_tree_add_item(field_tree, hf_tcp_option_kind, tvb,
                        offset, 1, ENC_BIG_ENDIAN);
    proto_tree_add_item(field_tree, hf_tcp_option_len, tvb,
                        offset + 1, 1, ENC_BIG_ENDIAN);

    offset += 2;    /* skip past type and length */
    optlen -= 2;    /* subtract size of type and length */

    while (optlen > 0) {
        if (optlen < 4) {
            proto_tree_add_expert(field_tree, pinfo, &ei_tcp_suboption_malformed, tvb, offset, optlen);
            break;
        }
        leftedge = tvb_get_ntohl(tvb, offset)-base_ack;
        proto_tree_add_uint_format(field_tree, hf_tcp_option_sack_sle, tvb,
                                   offset, 4, leftedge,
                                   "left edge = %u%s", leftedge,
                                   (tcp_analyze_seq && tcp_relative_seq) ? " (relative)" : "");

        optlen -= 4;
        if (optlen < 4) {
            proto_tree_add_expert(field_tree, pinfo, &ei_tcp_suboption_malformed, tvb, offset, optlen);
            break;
        }
        /* XXX - check whether it goes past end of packet */
        rightedge = tvb_get_ntohl(tvb, offset + 4)-base_ack;
        optlen -= 4;
        proto_tree_add_uint_format(field_tree, hf_tcp_option_sack_sre, tvb,
                                   offset+4, 4, rightedge,
                                   "right edge = %u%s", rightedge,
                                   (tcp_analyze_seq && tcp_relative_seq) ? " (relative)" : "");
        tcp_info_append_uint(pinfo, "SLE", leftedge);
        tcp_info_append_uint(pinfo, "SRE", rightedge);
        num_sack_ranges++;

        /* Update tap info */
        if (tcph != NULL && (tcph->num_sack_ranges < MAX_TCP_SACK_RANGES)) {
            tcph->sack_left_edge[tcph->num_sack_ranges] = leftedge;
            tcph->sack_right_edge[tcph->num_sack_ranges] = rightedge;
            tcph->num_sack_ranges++;
        }

        proto_item_append_text(field_tree, " %u-%u", leftedge, rightedge);
        offset += 8;
    }

    /* Show number of SACK ranges in this option as a generated field */
    tf = proto_tree_add_uint(field_tree, hf_tcp_option_sack_range_count,
                             tvb, 0, 0, num_sack_ranges);
    PROTO_ITEM_SET_GENERATED(tf);

    return tvb_captured_length(tvb);
}