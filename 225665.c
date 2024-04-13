scan_for_next_pdu(tvbuff_t *tvb, proto_tree *tcp_tree, packet_info *pinfo, int offset, guint32 seq, guint32 nxtseq, wmem_tree_t *multisegment_pdus)
{
    struct tcp_multisegment_pdu *msp=NULL;

    if(!pinfo->fd->visited) {
        msp=(struct tcp_multisegment_pdu *)wmem_tree_lookup32_le(multisegment_pdus, seq-1);
        if(msp) {
            /* If this is a continuation of a PDU started in a
             * previous segment we need to update the last_frame
             * variables.
            */
            if(seq>msp->seq && seq<msp->nxtpdu) {
                msp->last_frame=pinfo->num;
                msp->last_frame_time=pinfo->abs_ts;
                print_pdu_tracking_data(pinfo, tvb, tcp_tree, msp);
            }

            /* If this segment is completely within a previous PDU
             * then we just skip this packet
             */
            if(seq>msp->seq && nxtseq<=msp->nxtpdu) {
                return -1;
            }
            if(seq<msp->nxtpdu && nxtseq>msp->nxtpdu) {
                offset+=msp->nxtpdu-seq;
                return offset;
            }

        }
    } else {
        /* First we try to find the start and transfer time for a PDU.
         * We only print this for the very first segment of a PDU
         * and only for PDUs spanning multiple segments.
         * Se we look for if there was any multisegment PDU started
         * just BEFORE the end of this segment. I.e. either inside this
         * segment or in a previous segment.
         * Since this might also match PDUs that are completely within
         * this segment we also verify that the found PDU does span
         * beyond the end of this segment.
         */
        msp=(struct tcp_multisegment_pdu *)wmem_tree_lookup32_le(multisegment_pdus, nxtseq-1);
        if(msp) {
            if(pinfo->num==msp->first_frame) {
                proto_item *item;
                nstime_t ns;

                item=proto_tree_add_uint(tcp_tree, hf_tcp_pdu_last_frame, tvb, 0, 0, msp->last_frame);
                PROTO_ITEM_SET_GENERATED(item);

                nstime_delta(&ns, &msp->last_frame_time, &pinfo->abs_ts);
                item = proto_tree_add_time(tcp_tree, hf_tcp_pdu_time,
                        tvb, 0, 0, &ns);
                PROTO_ITEM_SET_GENERATED(item);
            }
        }

        /* Second we check if this segment is part of a PDU started
         * prior to the segment (seq-1)
         */
        msp=(struct tcp_multisegment_pdu *)wmem_tree_lookup32_le(multisegment_pdus, seq-1);
        if(msp) {
            /* If this segment is completely within a previous PDU
             * then we just skip this packet
             */
            if(seq>msp->seq && nxtseq<=msp->nxtpdu) {
                print_pdu_tracking_data(pinfo, tvb, tcp_tree, msp);
                return -1;
            }

            if(seq<msp->nxtpdu && nxtseq>msp->nxtpdu) {
                offset+=msp->nxtpdu-seq;
                return offset;
            }
        }

    }
    return offset;
}