sn_coap_hdr_s *sn_coap_parser(struct coap_s *handle, uint16_t packet_data_len, uint8_t *packet_data_ptr, coap_version_e *coap_version_ptr)
{
    uint8_t       *data_temp_ptr                    = packet_data_ptr;
    sn_coap_hdr_s *parsed_and_returned_coap_msg_ptr = NULL;

    /* * * * Check given pointer * * * */
    if (packet_data_ptr == NULL || packet_data_len < 4 || handle == NULL) {
        return NULL;
    }

    /* * * * Allocate and initialize CoAP message  * * * */
    parsed_and_returned_coap_msg_ptr = sn_coap_parser_alloc_message(handle);

    if (parsed_and_returned_coap_msg_ptr == NULL) {
        tr_error("sn_coap_parser - failed to allocate message!");
        return NULL;
    }

    /* * * * Header parsing, move pointer over the header...  * * * */
    sn_coap_parser_header_parse(&data_temp_ptr, parsed_and_returned_coap_msg_ptr, coap_version_ptr);
    /* * * * Options parsing, move pointer over the options... * * * */
    if (sn_coap_parser_options_parse(handle, &data_temp_ptr, parsed_and_returned_coap_msg_ptr, packet_data_ptr, packet_data_len) != 0) {
        parsed_and_returned_coap_msg_ptr->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
        return parsed_and_returned_coap_msg_ptr;
    }

    /* * * * Payload parsing * * * */
    if (sn_coap_parser_payload_parse(packet_data_len, packet_data_ptr, &data_temp_ptr, parsed_and_returned_coap_msg_ptr) == -1) {
        parsed_and_returned_coap_msg_ptr->coap_status = COAP_STATUS_PARSER_ERROR_IN_HEADER;
        return parsed_and_returned_coap_msg_ptr;
    }

    parsed_and_returned_coap_msg_ptr->coap_status = COAP_STATUS_OK;

    /* * * * Return parsed CoAP message  * * * * */
    return parsed_and_returned_coap_msg_ptr;
}