decode_tcp_ports(tvbuff_t *tvb, int offset, packet_info *pinfo,
    proto_tree *tree, int src_port, int dst_port,
    struct tcp_analysis *tcpd, struct tcpinfo *tcpinfo)
{
    tvbuff_t *next_tvb;
    int low_port, high_port;
    int save_desegment_offset;
    guint32 save_desegment_len;
    heur_dtbl_entry_t *hdtbl_entry;
    exp_pdu_data_t *exp_pdu_data;

    /* Don't call subdissectors for keepalives.  Even though they do contain
     * payload "data", it's just garbage.  Display any data the keepalive
     * packet might contain though.
     */
    if(tcpd && tcpd->ta) {
        if(tcpd->ta->flags&TCP_A_KEEP_ALIVE) {
            next_tvb = tvb_new_subset_remaining(tvb, offset);
            call_dissector(data_handle, next_tvb, pinfo, tree);
            return TRUE;
        }
    }

    if (tcp_no_subdissector_on_error && !(tcp_desegment && tcp_reassemble_out_of_order) &&
        tcpd && tcpd->ta && tcpd->ta->flags & (TCP_A_RETRANSMISSION | TCP_A_OUT_OF_ORDER)) {
        /* Don't try to dissect a retransmission high chance that it will mess
         * subdissectors for protocols that require in-order delivery of the
         * PDUs. (i.e. DCE/RPCoverHTTP and encryption)
         * If OoO reassembly is enabled and if this segment was previously lost,
         * then this retransmission could have finished reassembly, so continue.
         * XXX should this option be removed? "tcp_reassemble_out_of_order"
         * should have addressed the above in-order requirement.
         */
        return FALSE;
    }
    next_tvb = tvb_new_subset_remaining(tvb, offset);

    save_desegment_offset = pinfo->desegment_offset;
    save_desegment_len = pinfo->desegment_len;

/* determine if this packet is part of a conversation and call dissector */
/* for the conversation if available */

    if (try_conversation_dissector(&pinfo->src, &pinfo->dst, ENDPOINT_TCP,
                                   src_port, dst_port, next_tvb, pinfo, tree, tcpinfo, 0)) {
        pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
        handle_export_pdu_conversation(pinfo, next_tvb, src_port, dst_port, tcpinfo);
        return TRUE;
    }

    if (try_heuristic_first) {
        /* do lookup with the heuristic subdissector table */
        if (dissector_try_heuristic(heur_subdissector_list, next_tvb, pinfo, tree, &hdtbl_entry, tcpinfo)) {
            pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
            handle_export_pdu_heuristic(pinfo, next_tvb, hdtbl_entry, tcpinfo);
            return TRUE;
        }
    }

    /* Do lookups with the subdissector table.
       Try the server port captured on the SYN or SYN|ACK packet.  After that
       try the port number with the lower value first, followed by the
       port number with the higher value.  This means that, for packets
       where a dissector is registered for *both* port numbers:

       1) we pick the same dissector for traffic going in both directions;

       2) we prefer the port number that's more likely to be the right
       one (as that prefers well-known ports to reserved ports);

       although there is, of course, no guarantee that any such strategy
       will always pick the right port number.

       XXX - we ignore port numbers of 0, as some dissectors use a port
       number of 0 to disable the port. */

    if (tcpd && tcpd->server_port != 0 &&
        dissector_try_uint_new(subdissector_table, tcpd->server_port, next_tvb, pinfo, tree, TRUE, tcpinfo)) {
        pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
        handle_export_pdu_dissection_table(pinfo, next_tvb, tcpd->server_port, tcpinfo);
        return TRUE;
    }

    if (src_port > dst_port) {
        low_port = dst_port;
        high_port = src_port;
    } else {
        low_port = src_port;
        high_port = dst_port;
    }

    if (low_port != 0 &&
        dissector_try_uint_new(subdissector_table, low_port, next_tvb, pinfo, tree, TRUE, tcpinfo)) {
        pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
        handle_export_pdu_dissection_table(pinfo, next_tvb, low_port, tcpinfo);
        return TRUE;
    }
    if (high_port != 0 &&
        dissector_try_uint_new(subdissector_table, high_port, next_tvb, pinfo, tree, TRUE, tcpinfo)) {
        pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
        handle_export_pdu_dissection_table(pinfo, next_tvb, high_port, tcpinfo);
        return TRUE;
    }

    if (!try_heuristic_first) {
        /* do lookup with the heuristic subdissector table */
        if (dissector_try_heuristic(heur_subdissector_list, next_tvb, pinfo, tree, &hdtbl_entry, tcpinfo)) {
            pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
            handle_export_pdu_heuristic(pinfo, next_tvb, hdtbl_entry, tcpinfo);
            return TRUE;
        }
    }

    /*
     * heuristic / conversation / port registered dissectors rejected the packet;
     * make sure they didn't also request desegmentation (we could just override
     * the request, but rejecting a packet *and* requesting desegmentation is a sign
     * of the dissector's code needing clearer thought, so we fail so that the
     * problem is made more obvious).
     */
    DISSECTOR_ASSERT(save_desegment_offset == pinfo->desegment_offset &&
                     save_desegment_len == pinfo->desegment_len);

    /* Oh, well, we don't know this; dissect it as data. */
    call_dissector(data_handle,next_tvb, pinfo, tree);

    pinfo->want_pdu_tracking -= !!(pinfo->want_pdu_tracking);
    if (have_tap_listener(exported_pdu_tap)) {
        exp_pdu_data = export_pdu_create_common_tags(pinfo, "data", EXP_PDU_TAG_PROTO_NAME);
        exp_pdu_data->tvb_captured_length = tvb_captured_length(next_tvb);
        exp_pdu_data->tvb_reported_length = tvb_reported_length(next_tvb);
        exp_pdu_data->pdu_tvb = next_tvb;

        tap_queue_packet(exported_pdu_tap, pinfo, exp_pdu_data);
    }
    return FALSE;
}