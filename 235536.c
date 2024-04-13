sn_coap_hdr_s *sn_coap_parser_init_message(sn_coap_hdr_s *coap_msg_ptr)
{
    /* * * * Check given pointer * * * */
    if (coap_msg_ptr == NULL) {
        tr_error("sn_coap_parser_init_message - message null!");
        return NULL;
    }

    /* XXX not technically legal to memset pointers to 0 */
    memset(coap_msg_ptr, 0x00, sizeof(sn_coap_hdr_s));

    coap_msg_ptr->content_format = COAP_CT_NONE;

    return coap_msg_ptr;
}