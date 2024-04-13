void sn_coap_parser_release_allocated_coap_msg_mem(struct coap_s *handle, sn_coap_hdr_s *freed_coap_msg_ptr)
{
    if (handle == NULL) {
        return;
    }

    if (freed_coap_msg_ptr != NULL) {

        // As there are multiple sequential calls to the protocol_free, caching pointer to it
        // saves one instruction per call.
        void (*local_free)(void *) = handle->sn_coap_protocol_free;

        local_free(freed_coap_msg_ptr->uri_path_ptr);
        local_free(freed_coap_msg_ptr->token_ptr);

        // same here, caching the struct start saves a bit.
        sn_coap_options_list_s *options_list_ptr = freed_coap_msg_ptr->options_list_ptr;

        if (options_list_ptr != NULL) {

            local_free(options_list_ptr->proxy_uri_ptr);

            local_free(options_list_ptr->etag_ptr);

            local_free(options_list_ptr->uri_host_ptr);

            local_free(options_list_ptr->location_path_ptr);

            local_free(options_list_ptr->location_query_ptr);

            local_free(options_list_ptr->uri_query_ptr);

            local_free(options_list_ptr);
        }

        local_free(freed_coap_msg_ptr);
    }
}