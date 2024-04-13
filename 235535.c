static int8_t sn_coap_parser_options_parse_multiple_options(struct coap_s *handle, uint8_t **packet_data_pptr, uint16_t packet_left_len,  uint8_t **dst_pptr, uint16_t *dst_len_ptr, sn_coap_option_numbers_e option, uint16_t option_number_len)
{
    int16_t     uri_query_needed_heap       = sn_coap_parser_options_count_needed_memory_multiple_option(*packet_data_pptr, packet_left_len, option, option_number_len);
    uint8_t    *temp_parsed_uri_query_ptr   = NULL;
    uint8_t     returned_option_counter     = 0;
    uint8_t    *start_ptr = *packet_data_pptr;
    uint16_t    message_left = packet_left_len;

    if (uri_query_needed_heap == -1) {
        return -1;
    }

    if (uri_query_needed_heap) {
        *dst_pptr = (uint8_t *) handle->sn_coap_protocol_malloc(uri_query_needed_heap);

        if (*dst_pptr == NULL) {
            tr_error("sn_coap_parser_options_parse_multiple_options - failed to allocate options!");
            return -1;
        }
    }

    *dst_len_ptr = uri_query_needed_heap;
    temp_parsed_uri_query_ptr = *dst_pptr;

    /* Loop all Uri-Query options */
    while ((temp_parsed_uri_query_ptr - *dst_pptr) < uri_query_needed_heap
           && message_left) {
        /* Check if this is first Uri-Query option */
        if (returned_option_counter > 0) {
            /* Uri-Query is modified to following format: temp1'\0'temp2'\0'temp3 i.e.  */
            /* Uri-Path is modified to following format: temp1\temp2\temp3 i.e.  */
            if (option == COAP_OPTION_URI_QUERY || option == COAP_OPTION_LOCATION_QUERY || option == COAP_OPTION_ETAG || option == COAP_OPTION_ACCEPT) {
                *temp_parsed_uri_query_ptr = '&';
            } else if (option == COAP_OPTION_URI_PATH || option == COAP_OPTION_LOCATION_PATH) {
                *temp_parsed_uri_query_ptr = '/';
            }

            temp_parsed_uri_query_ptr++;
        }

        returned_option_counter++;

        if (((temp_parsed_uri_query_ptr - *dst_pptr) + option_number_len) > uri_query_needed_heap) {
            return -1;
        }

        if (0 != sn_coap_parser_check_packet_ptr(*packet_data_pptr, start_ptr, packet_left_len, option_number_len))
        {
            /* Bufer read overflow. */
            return -1;
        }

        /* Copy the option value to URI query buffer */
        memcpy(temp_parsed_uri_query_ptr, *packet_data_pptr, option_number_len);

        message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr,
                                                      start_ptr,
                                                      packet_left_len,
                                                      option_number_len);
        temp_parsed_uri_query_ptr += option_number_len;

        /* Check if there is more input to process */
        if ( message_left == 0 ||
            ((**packet_data_pptr >> COAP_OPTIONS_OPTION_NUMBER_SHIFT) != 0)) {
            return returned_option_counter;
        }

        /* Porcess next option */
        option_number_len = (**packet_data_pptr & 0x0F);
        message_left = sn_coap_parser_move_packet_ptr(packet_data_pptr, 
                                                      start_ptr, 
                                                      packet_left_len, 
                                                      1);

        /* Add possible option length extension to resolve full length of the option */
        int8_t option_parse_result = parse_ext_option(&option_number_len,
                                                      packet_data_pptr,
                                                      start_ptr,
                                                      packet_left_len,
                                                      &message_left);
        if (option_parse_result != 0)
        {
            /* Extended option parsing failed. */
            return -1;
        }
    }

    return returned_option_counter;
}