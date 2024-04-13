static int exp_pdu_tcp_dissector_data_populate_data(packet_info *pinfo _U_, void* data, guint8 *tlv_buffer, guint32 buffer_size _U_)
{
    struct tcpinfo* dissector_data = (struct tcpinfo*)data;

    tlv_buffer[0] = 0;
    tlv_buffer[1] = EXP_PDU_TAG_TCP_INFO_DATA;
    tlv_buffer[2] = 0;
    tlv_buffer[3] = EXP_PDU_TCP_INFO_DATA_LEN; /* tag length */
    tlv_buffer[4] = 0;
    tlv_buffer[5] = EXP_PDU_TCP_INFO_VERSION;
    tlv_buffer[6] = (dissector_data->seq & 0xff000000) >> 24;
    tlv_buffer[7] = (dissector_data->seq & 0x00ff0000) >> 16;
    tlv_buffer[8] = (dissector_data->seq & 0x0000ff00) >> 8;
    tlv_buffer[9] = (dissector_data->seq & 0x000000ff);
    tlv_buffer[10] = (dissector_data->nxtseq & 0xff000000) >> 24;
    tlv_buffer[11] = (dissector_data->nxtseq & 0x00ff0000) >> 16;
    tlv_buffer[12] = (dissector_data->nxtseq & 0x0000ff00) >> 8;
    tlv_buffer[13] = (dissector_data->nxtseq & 0x000000ff);
    tlv_buffer[14] = (dissector_data->lastackseq & 0xff000000) >> 24;
    tlv_buffer[15] = (dissector_data->lastackseq & 0x00ff0000) >> 16;
    tlv_buffer[16] = (dissector_data->lastackseq & 0x0000ff00) >> 8;
    tlv_buffer[17] = (dissector_data->lastackseq & 0x000000ff);
    tlv_buffer[18] = dissector_data->is_reassembled;
    tlv_buffer[19] = (dissector_data->flags & 0xff00) >> 8;
    tlv_buffer[20] = (dissector_data->flags & 0x00ff);
    tlv_buffer[21] = (dissector_data->urgent_pointer & 0xff00) >> 8;
    tlv_buffer[22] = (dissector_data->urgent_pointer & 0x00ff);

    return exp_pdu_tcp_dissector_data_size(pinfo, data);
}