missing_segments(packet_info *pinfo, struct tcp_multisegment_pdu *msp, guint32 seq)
{
    fragment_head *fd_head;
    guint32 frag_offset = seq - msp->seq;

    if ((gint32)frag_offset <= 0) {
        return FALSE;
    }

    fd_head = fragment_get(&tcp_reassembly_table, pinfo, msp->first_frame, NULL);
    /* msp implies existence of fragments, this should never be NULL. */
    DISSECTOR_ASSERT(fd_head);

    /* Find length of contiguous fragments. */
    guint32 max = 0;
    for (fragment_item *frag = fd_head; frag; frag = frag->next) {
        guint32 frag_end = frag->offset + frag->len;
        if (frag->offset <= max && max < frag_end) {
            max = frag_end;
        }
    }
    return max < frag_offset;
}