desegment_tcp(tvbuff_t *tvb, packet_info *pinfo, int offset,
              guint32 seq, guint32 nxtseq,
              guint32 sport, guint32 dport,
              proto_tree *tree, proto_tree *tcp_tree,
              struct tcp_analysis *tcpd, struct tcpinfo *tcpinfo)
{
    fragment_head *ipfd_head;
    int last_fragment_len;
    gboolean must_desegment;
    gboolean called_dissector;
    int another_pdu_follows;
    int deseg_offset;
    guint32 deseg_seq;
    gint nbytes;
    proto_item *item;
    struct tcp_multisegment_pdu *msp;
    gboolean cleared_writable = col_get_writable(pinfo->cinfo, COL_PROTOCOL);
    const gboolean reassemble_ooo = tcp_desegment && tcp_reassemble_out_of_order;

again:
    ipfd_head = NULL;
    last_fragment_len = 0;
    must_desegment = FALSE;
    called_dissector = FALSE;
    another_pdu_follows = 0;
    msp = NULL;

    /*
     * Initialize these to assume no desegmentation.
     * If that's not the case, these will be set appropriately
     * by the subdissector.
     */
    pinfo->desegment_offset = 0;
    pinfo->desegment_len = 0;

    /*
     * Initialize this to assume that this segment will just be
     * added to the middle of a desegmented chunk of data, so
     * that we should show it all as data.
     * If that's not the case, it will be set appropriately.
     */
    deseg_offset = offset;

    if (tcpd) {
        /* Have we seen this PDU before (and is it the start of a multi-
         * segment PDU)?
         *
         * If the sequence number was seen before, it is part of a
         * retransmission if the whole segment fits within the MSP.
         * (But if this is this frame was already visited and the first frame of
         * the MSP matches the current frame, then it is not a retransmission,
         * but the start of a new MSP.)
         *
         * If only part of the segment fits in the MSP, then either:
         * - The previous segment included with the MSP was a Zero Window Probe
         *   with one byte of data and the subdissector just asked for one more
         *   byte. Do not mark it as retransmission (Bug 15427).
         * - Data was actually being retransmitted, but with additional data
         *   (Bug 13523). Do not mark it as retransmission to handle the extra
         *   bytes. (NOTE Due to the TCP_A_RETRANSMISSION check below, such
         *   extra data will still be ignored.)
         * - The MSP contains multiple segments, but the subdissector finished
         *   reassembly using a subset of the final segment (thus "msp->nxtpdu"
         *   is smaller than the nxtseq of the previous segment). If that final
         *   segment was retransmitted, then "nxtseq > msp->nxtpdu".
         *   Unfortunately that will *not* be marked as retransmission here.
         *   The next TCP_A_RETRANSMISSION hopefully takes care of it though.
         *
         * Only shortcircuit here when the first segment of the MSP is known,
         * and when this this first segment is not one to complete the MSP.
         */
        if ((msp = (struct tcp_multisegment_pdu *)wmem_tree_lookup32(tcpd->fwd->multisegment_pdus, seq)) &&
                nxtseq <= msp->nxtpdu &&
                !(msp->flags & MSP_FLAGS_MISSING_FIRST_SEGMENT) && msp->last_frame != pinfo->num) {
            const char* str;
            gboolean is_retransmission = FALSE;

            /* Yes.  This could be because we've dissected this frame before
             * or because this is a retransmission of a previously-seen
             * segment.  Either way, we don't need to hand it off to the
             * subdissector and we certainly don't want to re-add it to the
             * multisegment_pdus list: if we did, subsequent lookups would
             * find this retransmission instead of the original transmission
             * (breaking desegmentation if we'd already linked other segments
             * to the original transmission's entry).
             *
             * Cases to handle here:
             * - In-order stream, pinfo->num matches begin of MSP.
             * - In-order stream, but pinfo->num does not match the begin of the
             *   MSP. Must be a retransmission.
             * - OoO stream where this segment fills the gap in the begin of the
             *   MSP. msp->first_frame is the start where the gap was detected
             *   (and does NOT match pinfo->num).
             */

            if (msp->first_frame == pinfo->num || msp->first_frame_with_seq == pinfo->num) {
                str = "";
                col_append_sep_str(pinfo->cinfo, COL_INFO, " ", "[TCP segment of a reassembled PDU]");
            } else {
                str = "Retransmitted ";
                is_retransmission = TRUE;
                /* TCP analysis already flags this (in COL_INFO) as a retransmission--if it's enabled */
            }

            /* Fix for bug 3264: look up ipfd for this (first) segment,
               so can add tcp.reassembled_in generated field on this code path. */
            if (!is_retransmission) {
                ipfd_head = fragment_get(&tcp_reassembly_table, pinfo, msp->first_frame, NULL);
                if (ipfd_head) {
                    if (ipfd_head->reassembled_in != 0) {
                        item = proto_tree_add_uint(tcp_tree, hf_tcp_reassembled_in, tvb, 0,
                                           0, ipfd_head->reassembled_in);
                        PROTO_ITEM_SET_GENERATED(item);
                    }
                }
            }

            nbytes = tvb_reported_length_remaining(tvb, offset);

            proto_tree_add_bytes_format(tcp_tree, hf_tcp_segment_data, tvb, offset,
                nbytes, NULL, "%sTCP segment data (%u byte%s)", str, nbytes,
                plurality(nbytes, "", "s"));
            return;
        }

        /* The above code only finds retransmission if the PDU boundaries and the seq coincide I think
         * If we have sequence analysis active use the TCP_A_RETRANSMISSION flag.
         * XXXX Could the above code be improved?
         * XXX the following check works great for filtering duplicate
         * retransmissions, but could there be a case where it prevents
         * "tcp_reassemble_out_of_order" from functioning due to skipping
         * retransmission of a lost segment?
         * If the latter is enabled, it could use use "maxnextseq" for ignoring
         * retransmitted single-segment PDUs (that would require storing
         * per-packet state (tcp_per_packet_data_t) to make it work for two-pass
         * and random access dissection). Retransmitted segments that are part
         * of a MSP should already be passed only once to subdissectors due to
         * the "reassembled_in" check below.
         * The following should also check for TCP_A_SPURIOUS_RETRANSMISSION to
         * address bug 10289.
         */
        if((tcpd->ta) && ((tcpd->ta->flags&TCP_A_RETRANSMISSION) == TCP_A_RETRANSMISSION)){
            const char* str = "Retransmitted ";
            nbytes = tvb_reported_length_remaining(tvb, offset);
            proto_tree_add_bytes_format(tcp_tree, hf_tcp_segment_data, tvb, offset,
                nbytes, NULL, "%sTCP segment data (%u byte%s)", str, nbytes,
                plurality(nbytes, "", "s"));
            return;
        }
        /* Else, find the most previous PDU starting before this sequence number */
        if (!msp) {
            msp = (struct tcp_multisegment_pdu *)wmem_tree_lookup32_le(tcpd->fwd->multisegment_pdus, seq-1);
        }
    }

    if (reassemble_ooo && tcpd && !PINFO_FD_VISITED(pinfo)) {
        /* If there is a gap between this segment and any previous ones (that
         * is, seqno is larger than the maximum expected seqno), then it is
         * possibly an out-of-order segment. The very first segment is expected
         * to be in-order though (otherwise captures starting in midst of a
         * connection would never be reassembled).
         */
        if (tcpd->fwd->maxnextseq) {
            /* Segments may be missing due to packet loss (assume later
             * retransmission) or out-of-order (assume it will appear later).
             *
             * Extend an unfinished MSP when (1) missing segments exist between
             * the start of the previous, (2) unfinished MSP and new segment.
             *
             * Create a new MSP when no (1) previous MSP exists and (2) a gap is
             * detected between the previous largest nxtseq and the new segment.
             */
            /* Whether a previous MSP exists with missing segments. */
            gboolean has_unfinished_msp = msp && !(msp->flags & MSP_FLAGS_GOT_ALL_SEGMENTS);
            /* Whether the new segment creates a new gap. */
            gboolean has_gap = LT_SEQ(tcpd->fwd->maxnextseq, seq);

            if (has_unfinished_msp && missing_segments(pinfo, msp, seq)) {
                /* The last PDU is part of a MSP which still needed more data,
                 * extend it (if necessary) to cover the entire new segment.
                 */
                if (LT_SEQ(msp->nxtpdu, nxtseq)) {
                    msp->nxtpdu = nxtseq;
                }
            } else if (!has_unfinished_msp && has_gap) {
                /* Either the previous segment was a single PDU that did not
                 * belong to a MSP, or the previous MSP was completed and cannot
                 * be extended.
                 * Create a new one starting at the expected next position and
                 * extend it to the end of the new segment.
                 */
                msp = pdu_store_sequencenumber_of_next_pdu(pinfo,
                    tcpd->fwd->maxnextseq, nxtseq,
                    tcpd->fwd->multisegment_pdus);

                msp->flags |= MSP_FLAGS_MISSING_FIRST_SEGMENT;
            }
            /* Now that the MSP is updated or created, continue adding the
             * segments to the MSP below. The subdissector will not be called as
             * the MSP is not complete yet. */
        }
        if (tcpd->fwd->maxnextseq == 0 || LT_SEQ(tcpd->fwd->maxnextseq, nxtseq)) {
            /* Update the maximum expected seqno if no SYN packet was seen
             * before, or if the new segment succeeds previous segments. */
            tcpd->fwd->maxnextseq = nxtseq;
        }
    }

    if (msp && msp->seq <= seq && msp->nxtpdu > seq) {
        int len;

        if (!PINFO_FD_VISITED(pinfo)) {
            msp->last_frame=pinfo->num;
            msp->last_frame_time=pinfo->abs_ts;
        }

        /* OK, this PDU was found, which means the segment continues
         * a higher-level PDU and that we must desegment it.
         */
        if (msp->flags&MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT) {
            /* The dissector asked for the entire segment */
            len = tvb_captured_length_remaining(tvb, offset);
        } else {
            len = MIN(nxtseq, msp->nxtpdu) - seq;
        }
        last_fragment_len = len;


        if (reassemble_ooo) {
            /*
             * If the previous segment requested more data (setting
             * FD_PARTIAL_REASSEMBLY as the next segment length is unknown), but
             * subsequently an OoO segment was received (for an earlier hole),
             * then "fragment_add" would truncate the reassembled PDU to the end
             * of this OoO segment. To prevent that, explicitly specify the MSP
             * length before calling "fragment_add".
             */
            fragment_reset_tot_len(&tcp_reassembly_table, pinfo,
                                   msp->first_frame, NULL,
                                   MAX(seq + len, msp->nxtpdu) - msp->seq);
        }

        ipfd_head = fragment_add(&tcp_reassembly_table, tvb, offset,
                                 pinfo, msp->first_frame, NULL,
                                 seq - msp->seq, len,
                                 (LT_SEQ (nxtseq,msp->nxtpdu)) );

        if (!PINFO_FD_VISITED(pinfo)
        && msp->flags & MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT) {
            msp->flags &= (~MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT);

            /* If we consumed the entire segment there is no
             * other pdu starting anywhere inside this segment.
             * So update nxtpdu to point at least to the start
             * of the next segment.
             * (If the subdissector asks for even more data we
             * will advance nxtpdu even further later down in
             * the code.)
             */
            msp->nxtpdu = nxtseq;
        }

        if (reassemble_ooo && !PINFO_FD_VISITED(pinfo)) {
            /* If the first segment of the MSP was seen, remember it. */
            if (msp->seq == seq && msp->flags & MSP_FLAGS_MISSING_FIRST_SEGMENT) {
                msp->first_frame_with_seq = pinfo->num;
                msp->flags &= ~MSP_FLAGS_MISSING_FIRST_SEGMENT;
            }
            /* Remember when all segments are ready to avoid subsequent
             * out-of-order packets from extending this MSP. If a subsdissector
             * needs more segments, the flag will be cleared below. */
            if (ipfd_head) {
                msp->flags |= MSP_FLAGS_GOT_ALL_SEGMENTS;
            }
        }

        if( (msp->nxtpdu < nxtseq)
        &&  (msp->nxtpdu >= seq)
        &&  (len > 0)) {
            another_pdu_follows=msp->nxtpdu - seq;
        }
    } else {
        /* This segment was not found in our table, so it doesn't
         * contain a continuation of a higher-level PDU.
         * Call the normal subdissector.
         */

        /*
         * Supply the sequence number of this segment. We set this here
         * because this segment could be after another in the same packet,
         * in which case seq was incremented at the end of the loop.
         */
        tcpinfo->seq = seq;

        process_tcp_payload(tvb, offset, pinfo, tree, tcp_tree,
                            sport, dport, 0, 0, FALSE, tcpd, tcpinfo);
        called_dissector = TRUE;

        /* Did the subdissector ask us to desegment some more data
         * before it could handle the packet?
         * If so we have to create some structures in our table but
         * this is something we only do the first time we see this
         * packet.
         */
        if(pinfo->desegment_len) {
            if (!PINFO_FD_VISITED(pinfo))
                must_desegment = TRUE;

            /*
             * Set "deseg_offset" to the offset in "tvb"
             * of the first byte of data that the
             * subdissector didn't process.
             */
            deseg_offset = offset + pinfo->desegment_offset;
        }

        /* Either no desegmentation is necessary, or this is
         * segment contains the beginning but not the end of
         * a higher-level PDU and thus isn't completely
         * desegmented.
         */
        ipfd_head = NULL;
    }


    /* is it completely desegmented? */
    if (ipfd_head) {
        /*
         * Yes, we think it is.
         * We only call subdissector for the last segment.
         * Note that the last segment may include more than what
         * we needed.
         */
        if(ipfd_head->reassembled_in == pinfo->num) {
            /*
             * OK, this is the last segment.
             * Let's call the subdissector with the desegmented
             * data.
             */
            tvbuff_t *next_tvb;
            int old_len;

            /* create a new TVB structure for desegmented data */
            next_tvb = tvb_new_chain(tvb, ipfd_head->tvb_data);

            /* add desegmented data to the data source list */
            add_new_data_source(pinfo, next_tvb, "Reassembled TCP");

            /*
             * Supply the sequence number of the first of the
             * reassembled bytes.
             */
            tcpinfo->seq = msp->seq;

            /* indicate that this is reassembled data */
            tcpinfo->is_reassembled = TRUE;

            /* call subdissector */
            process_tcp_payload(next_tvb, 0, pinfo, tree, tcp_tree, sport,
                                dport, 0, 0, FALSE, tcpd, tcpinfo);
            called_dissector = TRUE;

            /*
             * OK, did the subdissector think it was completely
             * desegmented, or does it think we need even more
             * data?
             */
            if (reassemble_ooo && !PINFO_FD_VISITED(pinfo) && pinfo->desegment_len) {
                /* "desegment_len" isn't 0, so it needs more data to extend the MSP. */
                msp->flags &= ~MSP_FLAGS_GOT_ALL_SEGMENTS;
            }
            old_len = (int)(tvb_reported_length(next_tvb) - last_fragment_len);
            if (pinfo->desegment_len &&
                pinfo->desegment_offset<=old_len) {
                /*
                 * "desegment_len" isn't 0, so it needs more
                 * data for something - and "desegment_offset"
                 * is before "old_len", so it needs more data
                 * to dissect the stuff we thought was
                 * completely desegmented (as opposed to the
                 * stuff at the beginning being completely
                 * desegmented, but the stuff at the end
                 * being a new higher-level PDU that also
                 * needs desegmentation).
                 *
                 * If "desegment_offset" is 0, then nothing in the reassembled
                 * TCP segments was dissected, so remove the data source.
                 */
                if (pinfo->desegment_offset == 0)
                    remove_last_data_source(pinfo);
                fragment_set_partial_reassembly(&tcp_reassembly_table,
                                                pinfo, msp->first_frame, NULL);

                /* Update msp->nxtpdu to point to the new next
                 * pdu boundary.
                 */
                if (pinfo->desegment_len == DESEGMENT_ONE_MORE_SEGMENT) {
                    /* We want reassembly of at least one
                     * more segment so set the nxtpdu
                     * boundary to one byte into the next
                     * segment.
                     * This means that the next segment
                     * will complete reassembly even if it
                     * is only one single byte in length.
                     * If this is an OoO segment, then increment the MSP end.
                     */
                    msp->nxtpdu = MAX(seq + tvb_reported_length_remaining(tvb, offset), msp->nxtpdu) + 1;
                    msp->flags |= MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT;
                } else if (pinfo->desegment_len == DESEGMENT_UNTIL_FIN) {
                    tcpd->fwd->flags |= TCP_FLOW_REASSEMBLE_UNTIL_FIN;
                } else {
                    if (seq + last_fragment_len >= msp->nxtpdu) {
                        /* This is the segment (overlapping) the end of the MSP. */
                        msp->nxtpdu = seq + last_fragment_len + pinfo->desegment_len;
                    } else {
                        /* This is a segment before the end of the MSP, so it
                         * must be an out-of-order segmented that completed the
                         * MSP. The requested additional data is relative to
                         * that end.
                         */
                        msp->nxtpdu += pinfo->desegment_len;
                    }
                }

                /* Since we need at least some more data
                 * there can be no pdu following in the
                 * tail of this segment.
                 */
                another_pdu_follows = 0;
                offset += last_fragment_len;
                seq += last_fragment_len;
                if (tvb_captured_length_remaining(tvb, offset) > 0)
                    goto again;
            } else {
                /*
                 * Show the stuff in this TCP segment as
                 * just raw TCP segment data.
                 */
                nbytes = another_pdu_follows > 0
                    ? another_pdu_follows
                    : tvb_reported_length_remaining(tvb, offset);
                proto_tree_add_bytes_format(tcp_tree, hf_tcp_segment_data, tvb, offset,
                    nbytes, NULL, "TCP segment data (%u byte%s)", nbytes,
                    plurality(nbytes, "", "s"));

                print_tcp_fragment_tree(ipfd_head, tree, tcp_tree, pinfo, next_tvb);

                /* Did the subdissector ask us to desegment
                 * some more data?  This means that the data
                 * at the beginning of this segment completed
                 * a higher-level PDU, but the data at the
                 * end of this segment started a higher-level
                 * PDU but didn't complete it.
                 *
                 * If so, we have to create some structures
                 * in our table, but this is something we
                 * only do the first time we see this packet.
                 */
                if(pinfo->desegment_len) {
                    if (!PINFO_FD_VISITED(pinfo))
                        must_desegment = TRUE;

                    /* The stuff we couldn't dissect
                     * must have come from this segment,
                     * so it's all in "tvb".
                     *
                     * "pinfo->desegment_offset" is
                     * relative to the beginning of
                     * "next_tvb"; we want an offset
                     * relative to the beginning of "tvb".
                     *
                     * First, compute the offset relative
                     * to the *end* of "next_tvb" - i.e.,
                     * the number of bytes before the end
                     * of "next_tvb" at which the
                     * subdissector stopped.  That's the
                     * length of "next_tvb" minus the
                     * offset, relative to the beginning
                     * of "next_tvb, at which the
                     * subdissector stopped.
                     */
                    deseg_offset = ipfd_head->datalen - pinfo->desegment_offset;

                    /* "tvb" and "next_tvb" end at the
                     * same byte of data, so the offset
                     * relative to the end of "next_tvb"
                     * of the byte at which we stopped
                     * is also the offset relative to
                     * the end of "tvb" of the byte at
                     * which we stopped.
                     *
                     * Convert that back into an offset
                     * relative to the beginning of
                     * "tvb", by taking the length of
                     * "tvb" and subtracting the offset
                     * relative to the end.
                     */
                    deseg_offset = tvb_reported_length(tvb) - deseg_offset;
                }
            }
        }
    }

    if (must_desegment) {
        /* If the dissector requested "reassemble until FIN"
         * just set this flag for the flow and let reassembly
         * proceed at normal.  We will check/pick up these
         * reassembled PDUs later down in dissect_tcp() when checking
         * for the FIN flag.
         */
        if (tcpd && pinfo->desegment_len == DESEGMENT_UNTIL_FIN) {
            tcpd->fwd->flags |= TCP_FLOW_REASSEMBLE_UNTIL_FIN;
        }
        /*
         * The sequence number at which the stuff to be desegmented
         * starts is the sequence number of the byte at an offset
         * of "deseg_offset" into "tvb".
         *
         * The sequence number of the byte at an offset of "offset"
         * is "seq", i.e. the starting sequence number of this
         * segment, so the sequence number of the byte at
         * "deseg_offset" is "seq + (deseg_offset - offset)".
         */
        deseg_seq = seq + (deseg_offset - offset);

        if (tcpd && ((nxtseq - deseg_seq) <= 1024*1024)
            && (!PINFO_FD_VISITED(pinfo))) {
            if(pinfo->desegment_len == DESEGMENT_ONE_MORE_SEGMENT) {
                /* The subdissector asked to reassemble using the
                 * entire next segment.
                 * Just ask reassembly for one more byte
                 * but set this msp flag so we can pick it up
                 * above.
                 */
                msp = pdu_store_sequencenumber_of_next_pdu(pinfo, deseg_seq,
                    nxtseq+1, tcpd->fwd->multisegment_pdus);
                msp->flags |= MSP_FLAGS_REASSEMBLE_ENTIRE_SEGMENT;
            } else {
                msp = pdu_store_sequencenumber_of_next_pdu(pinfo,
                    deseg_seq, nxtseq+pinfo->desegment_len, tcpd->fwd->multisegment_pdus);
            }

            /* add this segment as the first one for this new pdu */
            fragment_add(&tcp_reassembly_table, tvb, deseg_offset,
                         pinfo, msp->first_frame, NULL,
                         0, nxtseq - deseg_seq,
                         LT_SEQ(nxtseq, msp->nxtpdu));
        }
    }

    if (!called_dissector || pinfo->desegment_len != 0) {
        if (ipfd_head != NULL && ipfd_head->reassembled_in != 0 &&
            !(ipfd_head->flags & FD_PARTIAL_REASSEMBLY)) {
            /*
             * We know what frame this PDU is reassembled in;
             * let the user know.
             */
            item = proto_tree_add_uint(tcp_tree, hf_tcp_reassembled_in, tvb, 0,
                                       0, ipfd_head->reassembled_in);
            PROTO_ITEM_SET_GENERATED(item);
        }

        /*
         * Either we didn't call the subdissector at all (i.e.,
         * this is a segment that contains the middle of a
         * higher-level PDU, but contains neither the beginning
         * nor the end), or the subdissector couldn't dissect it
         * all, as some data was missing (i.e., it set
         * "pinfo->desegment_len" to the amount of additional
         * data it needs).
         */
        if (pinfo->desegment_offset == 0) {
            /*
             * It couldn't, in fact, dissect any of it (the
             * first byte it couldn't dissect is at an offset
             * of "pinfo->desegment_offset" from the beginning
             * of the payload, and that's 0).
             * Just mark this as TCP.
             */
            col_set_str(pinfo->cinfo, COL_PROTOCOL, "TCP");
            col_append_sep_str(pinfo->cinfo, COL_INFO, " ", "[TCP segment of a reassembled PDU]");
        }

        /*
         * Show what's left in the packet as just raw TCP segment
         * data.
         * XXX - remember what protocol the last subdissector
         * was, and report it as a continuation of that, instead?
         */
        nbytes = tvb_reported_length_remaining(tvb, deseg_offset);

        proto_tree_add_bytes_format(tcp_tree, hf_tcp_segment_data, tvb, deseg_offset,
            -1, NULL, "TCP segment data (%u byte%s)", nbytes,
            plurality(nbytes, "", "s"));
    }
    pinfo->can_desegment = 0;
    pinfo->desegment_offset = 0;
    pinfo->desegment_len = 0;

    if(another_pdu_follows) {
        /* there was another pdu following this one. */
        pinfo->can_desegment = 2;
        /* we also have to prevent the dissector from changing the
         * PROTOCOL and INFO columns since what follows may be an
         * incomplete PDU and we don't want it be changed back from
         *  <Protocol>   to <TCP>
         */
        col_set_fence(pinfo->cinfo, COL_INFO);
        cleared_writable |= col_get_writable(pinfo->cinfo, COL_PROTOCOL);
        col_set_writable(pinfo->cinfo, COL_PROTOCOL, FALSE);
        offset += another_pdu_follows;
        seq += another_pdu_follows;
        goto again;
    } else {
        /* remove any blocking set above otherwise the
         * proto,colinfo tap will break
         */
        if(cleared_writable) {
            col_set_writable(pinfo->cinfo, COL_PROTOCOL, TRUE);
        }
    }
}