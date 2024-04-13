handle_export_pdu_conversation(packet_info *pinfo, tvbuff_t *tvb, int src_port, int dst_port, struct tcpinfo *tcpinfo)
{
    if (have_tap_listener(exported_pdu_tap)) {
        conversation_t *conversation = find_conversation(pinfo->num, &pinfo->src, &pinfo->dst, ENDPOINT_TCP, src_port, dst_port, 0);
        if (conversation != NULL)
        {
            dissector_handle_t handle = (dissector_handle_t)wmem_tree_lookup32_le(conversation->dissector_tree, pinfo->num);
            if (handle != NULL)
            {
                exp_pdu_data_item_t exp_pdu_data_dissector_data = {exp_pdu_tcp_dissector_data_size, exp_pdu_tcp_dissector_data_populate_data, NULL};
                const exp_pdu_data_item_t *tcp_exp_pdu_items[] = {
                    &exp_pdu_data_src_ip,
                    &exp_pdu_data_dst_ip,
                    &exp_pdu_data_port_type,
                    &exp_pdu_data_src_port,
                    &exp_pdu_data_dst_port,
                    &exp_pdu_data_orig_frame_num,
                    &exp_pdu_data_dissector_data,
                    NULL
                };

                exp_pdu_data_t *exp_pdu_data;

                exp_pdu_data_dissector_data.data = tcpinfo;

                exp_pdu_data = export_pdu_create_tags(pinfo, dissector_handle_get_dissector_name(handle), EXP_PDU_TAG_PROTO_NAME, tcp_exp_pdu_items);
                exp_pdu_data->tvb_captured_length = tvb_captured_length(tvb);
                exp_pdu_data->tvb_reported_length = tvb_reported_length(tvb);
                exp_pdu_data->pdu_tvb = tvb;

                tap_queue_packet(exported_pdu_tap, pinfo, exp_pdu_data);
            }
        }
    }
}