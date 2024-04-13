tcp_analyze_get_acked_struct(guint32 frame, guint32 seq, guint32 ack, gboolean createflag, struct tcp_analysis *tcpd)
{

    wmem_tree_key_t key[4];

    key[0].length = 1;
    key[0].key = &frame;

    key[1].length = 1;
    key[1].key = &seq;

    key[2].length = 1;
    key[2].key = &ack;

    key[3].length = 0;
    key[3].key = NULL;

    if (!tcpd) {
        return;
    }

    tcpd->ta = (struct tcp_acked *)wmem_tree_lookup32_array(tcpd->acked_table, key);
    if((!tcpd->ta) && createflag) {
        tcpd->ta = wmem_new0(wmem_file_scope(), struct tcp_acked);
        wmem_tree_insert32_array(tcpd->acked_table, key, (void *)tcpd->ta);
    }
}