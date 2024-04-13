sn_coap_hdr_s *sn_coap_parser_alloc_message(struct coap_s *handle)
{
    sn_coap_hdr_s *returned_coap_msg_ptr;

    /* * * * Check given pointer * * * */
    if (handle == NULL) {
        return NULL;
    }

    /* * * * Allocate memory for returned CoAP message and initialize allocated memory with with default values  * * * */
    returned_coap_msg_ptr = handle->sn_coap_protocol_malloc(sizeof(sn_coap_hdr_s));

    return sn_coap_parser_init_message(returned_coap_msg_ptr);
}