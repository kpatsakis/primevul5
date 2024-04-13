static void sn_coap_parser_header_parse(uint8_t **packet_data_pptr, sn_coap_hdr_s *dst_coap_msg_ptr, coap_version_e *coap_version_ptr)
{
    /* Parse CoAP Version and message type*/
    *coap_version_ptr = (coap_version_e)(**packet_data_pptr & COAP_HEADER_VERSION_MASK);
    dst_coap_msg_ptr->msg_type = (sn_coap_msg_type_e)(**packet_data_pptr & COAP_HEADER_MSG_TYPE_MASK);
    (*packet_data_pptr) += 1;

    /* Parse Message code */
    dst_coap_msg_ptr->msg_code = (sn_coap_msg_code_e) **packet_data_pptr;
    (*packet_data_pptr) += 1;

    /* Parse Message ID */
    dst_coap_msg_ptr->msg_id = *(*packet_data_pptr + 1);
    dst_coap_msg_ptr->msg_id += **packet_data_pptr << COAP_HEADER_MSG_ID_MSB_SHIFT;
    (*packet_data_pptr) += 2;

}