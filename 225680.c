get_tcp_conversation_data(conversation_t *conv, packet_info *pinfo)
{
    int direction;
    struct tcp_analysis *tcpd;
    gboolean clear_ta = TRUE;

    /* Did the caller supply the conversation pointer? */
    if( conv==NULL ) {
        /* If the caller didn't supply a conversation, don't
         * clear the analysis, it may be needed */
        clear_ta = FALSE;
        conv = find_or_create_conversation(pinfo);
    }

    /* Get the data for this conversation */
    tcpd=(struct tcp_analysis *)conversation_get_proto_data(conv, proto_tcp);

    /* If the conversation was just created or it matched a
     * conversation with template options, tcpd will not
     * have been initialized. So, initialize
     * a new tcpd structure for the conversation.
     */
    if (!tcpd) {
        tcpd = init_tcp_conversation_data(pinfo);
        conversation_add_proto_data(conv, proto_tcp, tcpd);
    }

    if (!tcpd) {
      return NULL;
    }

    /* check direction and get ua lists */
    direction=cmp_address(&pinfo->src, &pinfo->dst);
    /* if the addresses are equal, match the ports instead */
    if(direction==0) {
        direction= (pinfo->srcport > pinfo->destport) ? 1 : -1;
    }
    if(direction>=0) {
        tcpd->fwd=&(tcpd->flow1);
        tcpd->rev=&(tcpd->flow2);
    } else {
        tcpd->fwd=&(tcpd->flow2);
        tcpd->rev=&(tcpd->flow1);
    }

    if (clear_ta) {
        tcpd->ta=NULL;
    }
    return tcpd;
}