process_tcp_payload(tvbuff_t *tvb, volatile int offset, packet_info *pinfo,
    proto_tree *tree, proto_tree *tcp_tree, int src_port, int dst_port,
    guint32 seq, guint32 nxtseq, gboolean is_tcp_segment,
    struct tcp_analysis *tcpd, struct tcpinfo *tcpinfo)
{
    pinfo->want_pdu_tracking=0;

    TRY {
        if(is_tcp_segment) {
            /*qqq   see if it is an unaligned PDU */
            if(tcpd && tcp_analyze_seq && (!tcp_desegment)) {
                if(seq || nxtseq) {
                    offset=scan_for_next_pdu(tvb, tcp_tree, pinfo, offset,
                        seq, nxtseq, tcpd->fwd->multisegment_pdus);
                }
            }
        }
        /* if offset is -1 this means that this segment is known
         * to be fully inside a previously detected pdu
         * so we don't even need to try to dissect it either.
         */
        if( (offset!=-1) &&
            decode_tcp_ports(tvb, offset, pinfo, tree, src_port,
                dst_port, tcpd, tcpinfo) ) {
            /*
             * We succeeded in handing off to a subdissector.
             *
             * Is this a TCP segment or a reassembled chunk of
             * TCP payload?
             */
            if(is_tcp_segment) {
                /* if !visited, check want_pdu_tracking and
                   store it in table */
                if(tcpd && (!pinfo->fd->visited) &&
                    tcp_analyze_seq && pinfo->want_pdu_tracking) {
                    if(seq || nxtseq) {
                        pdu_store_sequencenumber_of_next_pdu(
                            pinfo,
                            seq,
                            nxtseq+pinfo->bytes_until_next_pdu,
                            tcpd->fwd->multisegment_pdus);
                    }
                }
            }
        }
    }
    CATCH_ALL {
        /* We got an exception. At this point the dissection is
         * completely aborted and execution will be transferred back
         * to (probably) the frame dissector.
         * Here we have to place whatever we want the dissector
         * to do before aborting the tcp dissection.
         */
        /*
         * Is this a TCP segment or a reassembled chunk of TCP
         * payload?
         */
        if(is_tcp_segment) {
            /*
             * It's from a TCP segment.
             *
             * if !visited, check want_pdu_tracking and store it
             * in table
             */
            if(tcpd && (!pinfo->fd->visited) && tcp_analyze_seq && pinfo->want_pdu_tracking) {
                if(seq || nxtseq) {
                    pdu_store_sequencenumber_of_next_pdu(pinfo,
                        seq,
                        nxtseq+pinfo->bytes_until_next_pdu,
                        tcpd->fwd->multisegment_pdus);
                }
            }
        }
        RETHROW;
    }
    ENDTRY;
}