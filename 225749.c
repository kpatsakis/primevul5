pdu_store_sequencenumber_of_next_pdu(packet_info *pinfo, guint32 seq, guint32 nxtpdu, wmem_tree_t *multisegment_pdus)
{
    struct tcp_multisegment_pdu *msp;

    msp=wmem_new(wmem_file_scope(), struct tcp_multisegment_pdu);
    msp->nxtpdu=nxtpdu;
    msp->seq=seq;
    msp->first_frame=pinfo->num;
    msp->first_frame_with_seq=pinfo->num;
    msp->last_frame=pinfo->num;
    msp->last_frame_time=pinfo->abs_ts;
    msp->flags=0;
    wmem_tree_insert32(multisegment_pdus, seq, (void *)msp);
    /*g_warning("pdu_store_sequencenumber_of_next_pdu: seq %u", seq);*/
    return msp;
}