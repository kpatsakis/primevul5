sn_coap_options_list_s *sn_coap_parser_alloc_options(struct coap_s *handle, sn_coap_hdr_s *coap_msg_ptr)
{
    sn_coap_options_list_s *options_list_ptr;

    /* * * * Check given pointers * * * */
    if (handle == NULL || coap_msg_ptr == NULL) {
        return NULL;
    }

    /* * * * If the message already has options, return them * * * */
    if (coap_msg_ptr->options_list_ptr) {
        return coap_msg_ptr->options_list_ptr;
    }

    /* * * * Allocate memory for options and initialize allocated memory with with default values  * * * */
    /* XXX not technically legal to memset pointers to 0 */
    options_list_ptr = sn_coap_protocol_calloc(handle, sizeof(sn_coap_options_list_s));

    if (options_list_ptr == NULL) {
        tr_error("sn_coap_parser_alloc_options - failed to allocate options list!");
        return NULL;
    }

    coap_msg_ptr->options_list_ptr = options_list_ptr;

    options_list_ptr->uri_port = COAP_OPTION_URI_PORT_NONE;
    options_list_ptr->observe = COAP_OBSERVE_NONE;
    options_list_ptr->accept = COAP_CT_NONE;
    options_list_ptr->block2 = COAP_OPTION_BLOCK_NONE;
    options_list_ptr->block1 = COAP_OPTION_BLOCK_NONE;

    return options_list_ptr;
}