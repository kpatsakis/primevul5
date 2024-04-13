tcp_analyze_sequence_number(packet_info *pinfo, guint32 seq, guint32 ack, guint32 seglen, guint16 flags, guint32 window, struct tcp_analysis *tcpd)
{
    tcp_unacked_t *ual=NULL;
    tcp_unacked_t *prevual=NULL;
    guint32 nextseq;
    int ackcount;

#if 0
    printf("\nanalyze_sequence numbers   frame:%u\n",pinfo->num);
    printf("FWD list lastflags:0x%04x base_seq:%u: nextseq:%u lastack:%u\n",tcpd->fwd->lastsegmentflags,tcpd->fwd->base_seq,tcpd->fwd->tcp_analyze_seq_info->nextseq,tcpd->rev->tcp_analyze_seq_info->lastack);
    for(ual=tcpd->fwd->tcp_analyze_seq_info->segments; ual; ual=ual->next)
            printf("Frame:%d Seq:%u Nextseq:%u\n",ual->frame,ual->seq,ual->nextseq);
    printf("REV list lastflags:0x%04x base_seq:%u nextseq:%u lastack:%u\n",tcpd->rev->lastsegmentflags,tcpd->rev->base_seq,tcpd->rev->tcp_analyze_seq_info->nextseq,tcpd->fwd->tcp_analyze_seq_info->lastack);
    for(ual=tcpd->rev->tcp_analyze_seq_info->segments; ual; ual=ual->next)
            printf("Frame:%d Seq:%u Nextseq:%u\n",ual->frame,ual->seq,ual->nextseq);
#endif

    if (!tcpd) {
        return;
    }

    /* if this is the first segment for this list we need to store the
     * base_seq
     * We use TCP_S_SAW_SYN/SYNACK to distinguish between client and server
     *
     * Start relative seq and ack numbers at 1 if this
     * is not a SYN packet. This makes the relative
     * seq/ack numbers to be displayed correctly in the
     * event that the SYN or SYN/ACK packet is not seen
     * (this solves bug 1542)
     */
    if( !(tcpd->fwd->static_flags & TCP_S_BASE_SEQ_SET)) {
        if(flags & TH_SYN) {
            tcpd->fwd->base_seq = seq;
            tcpd->fwd->static_flags |= (flags & TH_ACK) ? TCP_S_SAW_SYNACK : TCP_S_SAW_SYN;
        }
        else {
            tcpd->fwd->base_seq = seq-1;
        }
        tcpd->fwd->static_flags |= TCP_S_BASE_SEQ_SET;
    }

    /* Only store reverse sequence if this isn't the SYN
     * There's no guarantee that the ACK field of a SYN
     * contains zeros; get the ISN from the first segment
     * with the ACK bit set instead (usually the SYN/ACK).
     *
     * If the SYN and SYN/ACK were received out-of-order,
     * the ISN is ack-1. If we missed the SYN/ACK, but got
     * the last ACK of the 3WHS, the ISN is ack-1. For all
     * other packets the ISN is unknown, so ack-1 is
     * as good a guess as ack.
     */
    if( !(tcpd->rev->static_flags & TCP_S_BASE_SEQ_SET) && (flags & TH_ACK) ) {
        tcpd->rev->base_seq = ack-1;
        tcpd->rev->static_flags |= TCP_S_BASE_SEQ_SET;
    }

    if( flags & TH_ACK ) {
        tcpd->rev->valid_bif = 1;
    }

    /* ZERO WINDOW PROBE
     * it is a zero window probe if
     *  the sequence number is the next expected one
     *  the window in the other direction is 0
     *  the segment is exactly 1 byte
     */
    if( seglen==1
    &&  seq==tcpd->fwd->tcp_analyze_seq_info->nextseq
    &&  tcpd->rev->window==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_ZERO_WINDOW_PROBE;
        goto finished_fwd;
    }


    /* ZERO WINDOW
     * a zero window packet has window == 0   but none of the SYN/FIN/RST set
     */
    if( window==0
    && (flags&(TH_RST|TH_FIN|TH_SYN))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_ZERO_WINDOW;
    }


    /* LOST PACKET
     * If this segment is beyond the last seen nextseq we must
     * have missed some previous segment
     *
     * We only check for this if we have actually seen segments prior to this
     * one.
     * RST packets are not checked for this.
     */
    if( tcpd->fwd->tcp_analyze_seq_info->nextseq
    &&  GT_SEQ(seq, tcpd->fwd->tcp_analyze_seq_info->nextseq)
    &&  (flags&(TH_RST))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_LOST_PACKET;

        /* Disable BiF until an ACK is seen in the other direction */
        tcpd->fwd->valid_bif = 0;
    }


    /* KEEP ALIVE
     * a keepalive contains 0 or 1 bytes of data and starts one byte prior
     * to what should be the next sequence number.
     * SYN/FIN/RST segments are never keepalives
     */
    if( (seglen==0||seglen==1)
    &&  seq==(tcpd->fwd->tcp_analyze_seq_info->nextseq-1)
    &&  (flags&(TH_SYN|TH_FIN|TH_RST))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_KEEP_ALIVE;
    }

    /* WINDOW UPDATE
     * A window update is a 0 byte segment with the same SEQ/ACK numbers as
     * the previous seen segment and with a new window value
     */
    if( seglen==0
    &&  window
    &&  window!=tcpd->fwd->window
    &&  seq==tcpd->fwd->tcp_analyze_seq_info->nextseq
    &&  ack==tcpd->fwd->tcp_analyze_seq_info->lastack
    &&  (flags&(TH_SYN|TH_FIN|TH_RST))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_WINDOW_UPDATE;
    }


    /* WINDOW FULL
     * If we know the window scaling
     * and if this segment contains data and goes all the way to the
     * edge of the advertised window
     * then we mark it as WINDOW FULL
     * SYN/RST/FIN packets are never WINDOW FULL
     */
    if( seglen>0
    &&  tcpd->rev->win_scale!=-1
    &&  (seq+seglen)==(tcpd->rev->tcp_analyze_seq_info->lastack+(tcpd->rev->window<<(tcpd->rev->win_scale==-2?0:tcpd->rev->win_scale)))
    &&  (flags&(TH_SYN|TH_FIN|TH_RST))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_WINDOW_FULL;
    }


    /* KEEP ALIVE ACK
     * It is a keepalive ack if it repeats the previous ACK and if
     * the last segment in the reverse direction was a keepalive
     */
    if( seglen==0
    &&  window
    &&  window==tcpd->fwd->window
    &&  seq==tcpd->fwd->tcp_analyze_seq_info->nextseq
    &&  ack==tcpd->fwd->tcp_analyze_seq_info->lastack
    && (tcpd->rev->lastsegmentflags&TCP_A_KEEP_ALIVE)
    &&  (flags&(TH_SYN|TH_FIN|TH_RST))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_KEEP_ALIVE_ACK;
        goto finished_fwd;
    }


    /* ZERO WINDOW PROBE ACK
     * It is a zerowindowprobe ack if it repeats the previous ACK and if
     * the last segment in the reverse direction was a zerowindowprobe
     * It also repeats the previous zero window indication
     */
    if( seglen==0
    &&  window==0
    &&  window==tcpd->fwd->window
    &&  seq==tcpd->fwd->tcp_analyze_seq_info->nextseq
    &&  ack==tcpd->fwd->tcp_analyze_seq_info->lastack
    && (tcpd->rev->lastsegmentflags&TCP_A_ZERO_WINDOW_PROBE)
    &&  (flags&(TH_SYN|TH_FIN|TH_RST))==0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_ZERO_WINDOW_PROBE_ACK;
        goto finished_fwd;
    }


    /* DUPLICATE ACK
     * It is a duplicate ack if window/seq/ack is the same as the previous
     * segment and if the segment length is 0
     */
    if( seglen==0
    &&  window
    &&  window==tcpd->fwd->window
    &&  seq==tcpd->fwd->tcp_analyze_seq_info->nextseq
    &&  ack==tcpd->fwd->tcp_analyze_seq_info->lastack
    &&  (flags&(TH_SYN|TH_FIN|TH_RST))==0 ) {
        tcpd->fwd->tcp_analyze_seq_info->dupacknum++;
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_DUPLICATE_ACK;
        tcpd->ta->dupack_num=tcpd->fwd->tcp_analyze_seq_info->dupacknum;
        tcpd->ta->dupack_frame=tcpd->fwd->tcp_analyze_seq_info->lastnondupack;
    }



finished_fwd:
    /* If the ack number changed we must reset the dupack counters */
    if( ack != tcpd->fwd->tcp_analyze_seq_info->lastack ) {
        tcpd->fwd->tcp_analyze_seq_info->lastnondupack=pinfo->num;
        tcpd->fwd->tcp_analyze_seq_info->dupacknum=0;
    }


    /* ACKED LOST PACKET
     * If this segment acks beyond the 'max seq to be acked' in the other direction
     * then that means we have missed packets going in the
     * other direction
     *
     * We only check this if we have actually seen some seq numbers
     * in the other direction.
     */
    if( tcpd->rev->tcp_analyze_seq_info->maxseqtobeacked
    &&  GT_SEQ(ack, tcpd->rev->tcp_analyze_seq_info->maxseqtobeacked )
    &&  (flags&(TH_ACK))!=0 ) {
        if(!tcpd->ta) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->flags|=TCP_A_ACK_LOST_PACKET;
        /* update 'max seq to be acked' in the other direction so we don't get
         * this indication again.
         */
        tcpd->rev->tcp_analyze_seq_info->maxseqtobeacked=tcpd->rev->tcp_analyze_seq_info->nextseq;
    }


    /* RETRANSMISSION/FAST RETRANSMISSION/OUT-OF-ORDER
     * If the segment contains data (or is a SYN or a FIN) and
     * if it does not advance the sequence number, it must be one
     * of these three.
     * Only test for this if we know what the seq number should be
     * (tcpd->fwd->nextseq)
     *
     * Note that a simple KeepAlive is not a retransmission
     */
    if (seglen>0 || flags&(TH_SYN|TH_FIN)) {
        gboolean seq_not_advanced = tcpd->fwd->tcp_analyze_seq_info->nextseq
                && (LT_SEQ(seq, tcpd->fwd->tcp_analyze_seq_info->nextseq));

        guint64 t;
        guint64 ooo_thres;

        if(tcpd->ta && (tcpd->ta->flags&TCP_A_KEEP_ALIVE) ) {
            goto finished_checking_retransmission_type;
        }

        /* This segment is *not* considered a retransmission/out-of-order if
         *  the segment length is larger than one (it really adds new data)
         *  the sequence number is one less than the previous nextseq and
         *      (the previous segment is possibly a zero window probe)
         *
         * We should still try to flag Spurious Retransmissions though.
         */
        if (seglen > 1 && tcpd->fwd->tcp_analyze_seq_info->nextseq - 1 == seq) {
            seq_not_advanced = FALSE;
        }

        /* If there were >=2 duplicate ACKs in the reverse direction
         * (there might be duplicate acks missing from the trace)
         * and if this sequence number matches those ACKs
         * and if the packet occurs within 20ms of the last
         * duplicate ack
         * then this is a fast retransmission
         */
        t=(pinfo->abs_ts.secs-tcpd->rev->tcp_analyze_seq_info->lastacktime.secs)*1000000000;
        t=t+(pinfo->abs_ts.nsecs)-tcpd->rev->tcp_analyze_seq_info->lastacktime.nsecs;
        if( seq_not_advanced
        &&  tcpd->rev->tcp_analyze_seq_info->dupacknum>=2
        &&  tcpd->rev->tcp_analyze_seq_info->lastack==seq
        &&  t<20000000 ) {
            if(!tcpd->ta) {
                tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
            }
            tcpd->ta->flags|=TCP_A_FAST_RETRANSMISSION;
            goto finished_checking_retransmission_type;
        }

        /* If the segment came relatively close since the segment with the highest
         * seen sequence number and it doesn't look like a retransmission
         * then it is an OUT-OF-ORDER segment.
         */
        t=(pinfo->abs_ts.secs-tcpd->fwd->tcp_analyze_seq_info->nextseqtime.secs)*1000000000;
        t=t+(pinfo->abs_ts.nsecs)-tcpd->fwd->tcp_analyze_seq_info->nextseqtime.nsecs;
        if (tcpd->ts_first_rtt.nsecs == 0 && tcpd->ts_first_rtt.secs == 0) {
            ooo_thres = 3000000;
        } else {
            ooo_thres = tcpd->ts_first_rtt.nsecs + tcpd->ts_first_rtt.secs*1000000000;
        }

        if( seq_not_advanced // XXX is this neccessary?
        && t < ooo_thres
        && tcpd->fwd->tcp_analyze_seq_info->nextseq != seq + seglen ) {
            if(!tcpd->ta) {
                tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
            }
            tcpd->ta->flags|=TCP_A_OUT_OF_ORDER;
            goto finished_checking_retransmission_type;
        }

        /* Check for spurious retransmission. If the current seq + segment length
         * is less than or equal to the current lastack, the packet contains
         * duplicate data and may be considered spurious.
         */
        if ( seglen > 0
        && tcpd->rev->tcp_analyze_seq_info->lastack
        && LE_SEQ(seq + seglen, tcpd->rev->tcp_analyze_seq_info->lastack) ) {
            if(!tcpd->ta){
                tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
            }
            tcpd->ta->flags|=TCP_A_SPURIOUS_RETRANSMISSION;
            goto finished_checking_retransmission_type;
        }

        if (seq_not_advanced) {
            /* Then it has to be a generic retransmission */
            if(!tcpd->ta) {
                tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
            }
            tcpd->ta->flags|=TCP_A_RETRANSMISSION;
            nstime_delta(&tcpd->ta->rto_ts, &pinfo->abs_ts, &tcpd->fwd->tcp_analyze_seq_info->nextseqtime);
            tcpd->ta->rto_frame=tcpd->fwd->tcp_analyze_seq_info->nextseqframe;
        }
    }

finished_checking_retransmission_type:

    nextseq = seq+seglen;
    if ((seglen || flags&(TH_SYN|TH_FIN)) && tcpd->fwd->tcp_analyze_seq_info->segment_count < TCP_MAX_UNACKED_SEGMENTS) {
        /* Add this new sequence number to the fwd list.  But only if there
         * aren't "too many" unacked segments (e.g., we're not seeing the ACKs).
         */
        ual = wmem_new(wmem_file_scope(), tcp_unacked_t);
        ual->next=tcpd->fwd->tcp_analyze_seq_info->segments;
        tcpd->fwd->tcp_analyze_seq_info->segments=ual;
        tcpd->fwd->tcp_analyze_seq_info->segment_count++;
        ual->frame=pinfo->num;
        ual->seq=seq;
        ual->ts=pinfo->abs_ts;

        /* next sequence number is seglen bytes away, plus SYN/FIN which counts as one byte */
        if( (flags&(TH_SYN|TH_FIN)) ) {
            nextseq+=1;
        }
        ual->nextseq=nextseq;
    }

    /* Store the highest number seen so far for nextseq so we can detect
     * when we receive segments that arrive with a "hole"
     * If we don't have anything since before, just store what we got.
     * ZeroWindowProbes are special and don't really advance the nextseq
     */
    if(GT_SEQ(nextseq, tcpd->fwd->tcp_analyze_seq_info->nextseq) || !tcpd->fwd->tcp_analyze_seq_info->nextseq) {
        if( !tcpd->ta || !(tcpd->ta->flags&TCP_A_ZERO_WINDOW_PROBE) ) {
            tcpd->fwd->tcp_analyze_seq_info->nextseq=nextseq;
            tcpd->fwd->tcp_analyze_seq_info->nextseqframe=pinfo->num;
            tcpd->fwd->tcp_analyze_seq_info->nextseqtime.secs=pinfo->abs_ts.secs;
            tcpd->fwd->tcp_analyze_seq_info->nextseqtime.nsecs=pinfo->abs_ts.nsecs;
        }
    }

    /* Store the highest continuous seq number seen so far for 'max seq to be acked',
     so we can detect TCP_A_ACK_LOST_PACKET condition
     */
    if(EQ_SEQ(seq, tcpd->fwd->tcp_analyze_seq_info->maxseqtobeacked) || !tcpd->fwd->tcp_analyze_seq_info->maxseqtobeacked) {
        if( !tcpd->ta || !(tcpd->ta->flags&TCP_A_ZERO_WINDOW_PROBE) ) {
            tcpd->fwd->tcp_analyze_seq_info->maxseqtobeacked=tcpd->fwd->tcp_analyze_seq_info->nextseq;
        }
    }


    /* remember what the ack/window is so we can track window updates and retransmissions */
    tcpd->fwd->window=window;
    tcpd->fwd->tcp_analyze_seq_info->lastack=ack;
    tcpd->fwd->tcp_analyze_seq_info->lastacktime.secs=pinfo->abs_ts.secs;
    tcpd->fwd->tcp_analyze_seq_info->lastacktime.nsecs=pinfo->abs_ts.nsecs;


    /* if there were any flags set for this segment we need to remember them
     * we only remember the flags for the very last segment though.
     */
    if(tcpd->ta) {
        tcpd->fwd->lastsegmentflags=tcpd->ta->flags;
    } else {
        tcpd->fwd->lastsegmentflags=0;
    }


    /* remove all segments this ACKs and we don't need to keep around any more
     */
    ackcount=0;
    prevual = NULL;
    ual = tcpd->rev->tcp_analyze_seq_info->segments;
    while(ual) {
        tcp_unacked_t *tmpual;

        /* If this ack matches the segment, process accordingly */
        if(ack==ual->nextseq) {
            tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
            tcpd->ta->frame_acked=ual->frame;
            nstime_delta(&tcpd->ta->ts, &pinfo->abs_ts, &ual->ts);
        }
        /* If this acknowledges part of the segment, adjust the segment info for the acked part */
        else if (GT_SEQ(ack, ual->seq) && LE_SEQ(ack, ual->nextseq)) {
            ual->seq = ack;
            continue;
        }
        /* If this acknowledges a segment prior to this one, leave this segment alone and move on */
        else if (GT_SEQ(ual->nextseq,ack)) {
            prevual = ual;
            ual = ual->next;
            continue;
        }

        /* This segment is old, or an exact match.  Delete the segment from the list */
        ackcount++;
        tmpual=ual->next;

        if (tcpd->rev->scps_capable) {
          /* Track largest segment successfully sent for SNACK analysis*/
          if ((ual->nextseq - ual->seq) > tcpd->fwd->maxsizeacked) {
            tcpd->fwd->maxsizeacked = (ual->nextseq - ual->seq);
          }
        }

        if (!prevual) {
            tcpd->rev->tcp_analyze_seq_info->segments = tmpual;
        }
        else{
            prevual->next = tmpual;
        }
        wmem_free(wmem_file_scope(), ual);
        ual = tmpual;
        tcpd->rev->tcp_analyze_seq_info->segment_count--;
    }

    /* how many bytes of data are there in flight after this frame
     * was sent
     */
    ual=tcpd->fwd->tcp_analyze_seq_info->segments;
    if (tcp_track_bytes_in_flight && seglen!=0 && ual && tcpd->fwd->valid_bif) {
        guint32 first_seq, last_seq, in_flight;

        first_seq = ual->seq - tcpd->fwd->base_seq;
        last_seq = ual->nextseq - tcpd->fwd->base_seq;
        while (ual) {
            if ((ual->nextseq-tcpd->fwd->base_seq)>last_seq) {
                last_seq = ual->nextseq-tcpd->fwd->base_seq;
            }
            if ((ual->seq-tcpd->fwd->base_seq)<first_seq) {
                first_seq = ual->seq-tcpd->fwd->base_seq;
            }
            ual = ual->next;
        }
        in_flight = last_seq-first_seq;

        if (in_flight>0 && in_flight<2000000000) {
            if(!tcpd->ta) {
                tcp_analyze_get_acked_struct(pinfo->num, seq, ack, TRUE, tcpd);
            }
            tcpd->ta->bytes_in_flight = in_flight;
        }

        if((flags & TH_PUSH) && !tcpd->fwd->push_set_last) {
          tcpd->fwd->push_bytes_sent += seglen;
          tcpd->fwd->push_set_last = TRUE;
        } else if ((flags & TH_PUSH) && tcpd->fwd->push_set_last) {
          tcpd->fwd->push_bytes_sent = seglen;
          tcpd->fwd->push_set_last = TRUE;
        } else if (tcpd->fwd->push_set_last) {
          tcpd->fwd->push_bytes_sent = seglen;
          tcpd->fwd->push_set_last = FALSE;
        } else {
          tcpd->fwd->push_bytes_sent += seglen;
        }
        if(!tcpd->ta) {
          tcp_analyze_get_acked_struct(pinfo->fd->num, seq, ack, TRUE, tcpd);
        }
        tcpd->ta->push_bytes_sent = tcpd->fwd->push_bytes_sent;
    }

}