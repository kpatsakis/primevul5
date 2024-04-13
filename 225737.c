check_follow_fragments(follow_info_t *follow_info, gboolean is_server, guint32 acknowledged, guint32 packet_num)
{
    GList *fragment_entry;
    follow_record_t *fragment, *follow_record;
    guint32 lowest_seq = 0;
    gchar *dummy_str;

    fragment_entry = g_list_first(follow_info->fragments[is_server]);
    if (fragment_entry == NULL)
        return FALSE;

    fragment = (follow_record_t*)fragment_entry->data;
    lowest_seq = fragment->seq;

    for (; fragment_entry != NULL; fragment_entry = g_list_next(fragment_entry))
    {
        fragment = (follow_record_t*)fragment_entry->data;

        if( GT_SEQ(lowest_seq, fragment->seq) ) {
            lowest_seq = fragment->seq;
        }

        if( LT_SEQ(fragment->seq, follow_info->seq[is_server]) ) {
            guint32 newseq;
            /* this sequence number seems dated, but
               check the end to make sure it has no more
               info than we have already seen */
            newseq = fragment->seq + fragment->data->len;
            if( GT_SEQ(newseq, follow_info->seq[is_server]) ) {
                guint32 new_pos;

                /* this one has more than we have seen. let's get the
                   payload that we have not seen. This happens when
                   part of this frame has been retransmitted */

                new_pos = follow_info->seq[is_server] - fragment->seq;

                if ( fragment->data->len > new_pos ) {
                    guint32 new_frag_size = fragment->data->len - new_pos;

                    follow_record = g_new0(follow_record_t,1);

                    follow_record->is_server = is_server;
                    follow_record->packet_num = fragment->packet_num;
                    follow_record->seq = follow_info->seq[is_server] + new_frag_size;

                    follow_record->data = g_byte_array_append(g_byte_array_new(),
                                                              fragment->data->data + new_pos,
                                                              new_frag_size);

                    follow_info->payload = g_list_prepend(follow_info->payload, follow_record);
                }

                follow_info->seq[is_server] += (fragment->data->len - new_pos);
            }

            /* Remove the fragment from the list as the "new" part of it
             * has been processed or its data has been seen already in
             * another packet. */
            g_byte_array_free(fragment->data, TRUE);
            g_free(fragment);
            follow_info->fragments[is_server] = g_list_delete_link(follow_info->fragments[is_server], fragment_entry);
            return TRUE;
        }

        if( EQ_SEQ(fragment->seq, follow_info->seq[is_server]) ) {
            /* this fragment fits the stream */
            if( fragment->data->len > 0 ) {
                follow_info->payload = g_list_prepend(follow_info->payload, fragment);
            }

            follow_info->seq[is_server] += fragment->data->len;
            follow_info->fragments[is_server] = g_list_delete_link(follow_info->fragments[is_server], fragment_entry);
            return TRUE;
        }
    }

    if( GT_SEQ(acknowledged, lowest_seq) ) {
        /* There are frames missing in the capture file that were seen
         * by the receiving host. Add dummy stream chunk with the data
         * "[xxx bytes missing in capture file]".
         */
        dummy_str = g_strdup_printf("[%d bytes missing in capture file]",
                        (int)(lowest_seq - follow_info->seq[is_server]) );
        // XXX the dummy replacement could be larger than the actual missing bytes.

        follow_record = g_new0(follow_record_t,1);

        follow_record->data = g_byte_array_append(g_byte_array_new(),
                                                  dummy_str,
                                                  (guint)strlen(dummy_str)+1);
        g_free(dummy_str);
        follow_record->is_server = is_server;
        follow_record->packet_num = packet_num;
        follow_record->seq = lowest_seq;

        follow_info->seq[is_server] = lowest_seq;
        follow_info->payload = g_list_prepend(follow_info->payload, follow_record);
        return TRUE;
    }

    return FALSE;
}