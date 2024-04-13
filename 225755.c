add_tcp_process_info(guint32 frame_num, address *local_addr, address *remote_addr, guint16 local_port, guint16 remote_port, guint32 uid, guint32 pid, gchar *username, gchar *command) {
    conversation_t *conv;
    struct tcp_analysis *tcpd;
    tcp_flow_t *flow = NULL;

    if (!tcp_display_process_info)
        return;

    conv = find_conversation(frame_num, local_addr, remote_addr, ENDPOINT_TCP, local_port, remote_port, 0);
    if (!conv) {
        return;
    }

    tcpd = (struct tcp_analysis *)conversation_get_proto_data(conv, proto_tcp);
    if (!tcpd) {
        return;
    }

    if (cmp_address(local_addr, conversation_key_addr1(conv->key_ptr)) == 0 && local_port == conversation_key_port1(conv->key_ptr)) {
        flow = &tcpd->flow1;
    } else if (cmp_address(remote_addr, conversation_key_addr1(conv->key_ptr)) == 0 && remote_port == conversation_key_port1(conv->key_ptr)) {
        flow = &tcpd->flow2;
    }
    if (!flow || (flow->process_info && flow->process_info->command)) {
        return;
    }

    if (flow->process_info == NULL)
        flow->process_info = wmem_new0(wmem_file_scope(), struct tcp_process_info_t);

    flow->process_info->process_uid = uid;
    flow->process_info->process_pid = pid;
    flow->process_info->username = wmem_strdup(wmem_file_scope(), username);
    flow->process_info->command = wmem_strdup(wmem_file_scope(), command);
}