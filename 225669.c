follow_tcp_tap_listener(void *tapdata, packet_info *pinfo,
                      epan_dissect_t *edt _U_, const void *data)
{
    follow_record_t *follow_record;
    follow_info_t *follow_info = (follow_info_t *)tapdata;
    const tcp_follow_tap_data_t *follow_data = (const tcp_follow_tap_data_t *)data;
    gboolean is_server;
    guint32 sequence = follow_data->tcph->th_seq;
    guint32 length = follow_data->tcph->th_seglen;
    guint32 data_offset = 0;
    guint32 data_length = tvb_captured_length(follow_data->tvb);

    if (follow_data->tcph->th_flags & TH_SYN) {
        sequence++;
    }

    if (follow_info->client_port == 0) {
        follow_info->client_port = pinfo->srcport;
        copy_address(&follow_info->client_ip, &pinfo->src);
        follow_info->server_port = pinfo->destport;
        copy_address(&follow_info->server_ip, &pinfo->dst);
    }

    is_server = !(addresses_equal(&follow_info->client_ip, &pinfo->src) && follow_info->client_port == pinfo->srcport);

   /* Check whether this frame ACKs fragments in flow from the other direction.
    * This happens when frames are not in the capture file, but were actually
    * seen by the receiving host (Fixes bug 592).
    */
    if (follow_info->fragments[!is_server] != NULL) {
        while (check_follow_fragments(follow_info, !is_server, follow_data->tcph->th_ack, pinfo->fd->num));
    }

    /*
     * If this is the first segment of this stream, initialize the next expected
     * sequence number. If there is any data, it will be added below.
     */
    if (follow_info->bytes_written[is_server] == 0 && follow_info->seq[is_server] == 0) {
        follow_info->seq[is_server] = sequence;
    }

    /* We have already seen this src (and received some segments), let's figure
     * out whether this segment extends the stream or overlaps a previous gap. */
    if (LT_SEQ(sequence, follow_info->seq[is_server])) {
        /* This sequence number seems dated, but check the end in case it was a
         * retransmission with more data. */
        guint32 nextseq = sequence + length;
        if (GT_SEQ(nextseq, follow_info->seq[is_server])) {
            /* The begin of the segment was already seen, try to add the
             * remaining data that we have not seen to the payload. */
            data_offset = follow_info->seq[is_server] - sequence;
            if (data_length <= data_offset) {
                data_length = 0;
            } else {
                data_length -= data_offset;
            }

            sequence = follow_info->seq[is_server];
            length = nextseq - follow_info->seq[is_server];
        }
    }
    /*
     * Ignore segments that have no new data (either because it was empty, or
     * because it was fully overlapping with previously received data).
     */
    if (data_length == 0 || LT_SEQ(sequence, follow_info->seq[is_server])) {
        return TAP_PACKET_DONT_REDRAW;
    }

    follow_record = g_new0(follow_record_t, 1);
    follow_record->is_server = is_server;
    follow_record->packet_num = pinfo->fd->num;
    follow_record->seq = sequence;  /* start of fragment, used by check_follow_fragments. */
    follow_record->data = g_byte_array_append(g_byte_array_new(),
                                              tvb_get_ptr(follow_data->tvb, data_offset, data_length),
                                              data_length);

    if (EQ_SEQ(sequence, follow_info->seq[is_server])) {
        /* The segment overlaps or extends the previous end of stream. */
        follow_info->seq[is_server] += length;
        follow_info->bytes_written[is_server] += follow_record->data->len;
        follow_info->payload = g_list_prepend(follow_info->payload, follow_record);

        /* done with the packet, see if it caused a fragment to fit */
        while(check_follow_fragments(follow_info, is_server, 0, pinfo->fd->num));
    } else {
        /* Out of order packet (more preceding segments are expected). */
        follow_info->fragments[is_server] = g_list_append(follow_info->fragments[is_server], follow_record);
    }
    return TAP_PACKET_DONT_REDRAW;
}