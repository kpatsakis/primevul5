static void reds_handle_auth_startlen(void *opaque)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    AsyncRead *obj = &link->async_read;
    RedsSASL *sasl = &link->stream->sasl;

    spice_info("Got client start len %d", sasl->len);
    if (sasl->len > SASL_DATA_MAX_LEN) {
        spice_warning("Too much SASL data %d", sasl->len);
        reds_send_link_error(link, SPICE_LINK_ERR_INVALID_DATA);
        reds_link_free(link);
        return;
    }

    if (sasl->len == 0) {
        reds_handle_auth_sasl_start(opaque);
        return;
    }

    sasl->data = spice_realloc(sasl->data, sasl->len);
    obj->now = (uint8_t *)sasl->data;
    obj->end = obj->now + sasl->len;
    obj->done = reds_handle_auth_sasl_start;
    async_read_handler(0, 0, &link->async_read);
}
