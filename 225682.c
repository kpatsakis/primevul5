dissect_tcp_payload(tvbuff_t *tvb, packet_info *pinfo, int offset, guint32 seq,
            guint32 nxtseq, guint32 sport, guint32 dport,
            proto_tree *tree, proto_tree *tcp_tree,
            struct tcp_analysis *tcpd, struct tcpinfo *tcpinfo)
{
    gint nbytes;
    gboolean save_fragmented;

    nbytes = tvb_reported_length_remaining(tvb, offset);
    proto_tree_add_bytes_format(tcp_tree, hf_tcp_payload, tvb, offset,
        -1, NULL, "TCP payload (%u byte%s)", nbytes,
        plurality(nbytes, "", "s"));

    /* Can we desegment this segment? */
    if (pinfo->can_desegment) {
        /* Yes. */
        desegment_tcp(tvb, pinfo, offset, seq, nxtseq, sport, dport, tree,
                      tcp_tree, tcpd, tcpinfo);
    } else {
        /* No - just call the subdissector.
           Mark this as fragmented, so if somebody throws an exception,
           we don't report it as a malformed frame. */
        save_fragmented = pinfo->fragmented;
        pinfo->fragmented = TRUE;

        process_tcp_payload(tvb, offset, pinfo, tree, tcp_tree, sport, dport,
                            seq, nxtseq, TRUE, tcpd, tcpinfo);
        pinfo->fragmented = save_fragmented;
    }
}