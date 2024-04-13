sn_coap_hdr_s *sn_coap_parser_alloc_message_with_options(struct coap_s *handle)
{
    // check the handle just as in any other place
    if (handle == NULL) {
        return NULL;
    }

    sn_coap_hdr_s *coap_msg_ptr = sn_coap_parser_alloc_message(handle);

    sn_coap_options_list_s *options_list_ptr = sn_coap_parser_alloc_options(handle, coap_msg_ptr);

    if ((coap_msg_ptr == NULL) || (options_list_ptr == NULL)) {

        // oops, out of memory free if got already any
        handle->sn_coap_protocol_free(coap_msg_ptr);
        handle->sn_coap_protocol_free(options_list_ptr);

        coap_msg_ptr = NULL;
    }

    return coap_msg_ptr;
}