static int8_t sn_coap_parser_payload_parse(uint16_t packet_data_len, uint8_t *packet_data_start_ptr, uint8_t **packet_data_pptr, sn_coap_hdr_s *dst_coap_msg_ptr)
{
    /* If there is payload */
    if ((*packet_data_pptr - packet_data_start_ptr) < packet_data_len) {
        if (**packet_data_pptr == 0xff) {
            (*packet_data_pptr)++;
            /* Parse Payload length */
            dst_coap_msg_ptr->payload_len = packet_data_len - (*packet_data_pptr - packet_data_start_ptr);

            /* The presence of a marker followed by a zero-length payload MUST be processed as a message format error */
            if (dst_coap_msg_ptr->payload_len == 0) {
                return -1;
            }

            /* Parse Payload by setting CoAP message's payload_ptr to point Payload in Packet data */
            dst_coap_msg_ptr->payload_ptr = *packet_data_pptr;
        }
        /* No payload marker.. */
        else {
            tr_error("sn_coap_parser_payload_parse - payload marker not found!");
            return -1;
        }
    }
    return 0;
}