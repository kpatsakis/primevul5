dissect_tcpopt_tfo_payload(tvbuff_t *tvb, int offset, guint optlen,
    packet_info *pinfo, proto_tree *exp_tree, void *data)
{
    proto_item *ti;
    struct tcpheader *tcph = (struct tcpheader*)data;

    if (optlen == 2) {
        /* Fast Open Cookie Request */
        proto_tree_add_item(exp_tree, hf_tcp_option_fast_open_cookie_request,
                            tvb, offset, 2, ENC_NA);
        col_append_str(pinfo->cinfo, COL_INFO, " TFO=R");
    } else if (optlen > 2) {
        /* Fast Open Cookie */
        ti = proto_tree_add_item(exp_tree, hf_tcp_option_fast_open_cookie,
                            tvb, offset + 2, optlen - 2, ENC_NA);
        col_append_str(pinfo->cinfo, COL_INFO, " TFO=C");
        if(tcph->th_flags & TH_SYN) {
            if((tcph->th_flags & TH_ACK) == 0) {
                expert_add_info(pinfo, ti, &ei_tcp_analysis_tfo_syn);
            }
        }
    }
}