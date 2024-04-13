static void reds_handle_auth_mechlen(void *opaque)
{
    RedLinkInfo *link = (RedLinkInfo *)opaque;
    AsyncRead *obj = &link->async_read;
    RedsSASL *sasl = &link->stream->sasl;

    if (sasl->len < 1 || sasl->len > 100) {
        spice_warning("Got bad client mechname len %d", sasl->len);
        reds_link_free(link);
        return;
    }

    sasl->mechname = spice_malloc(sasl->len + 1);

    spice_info("Wait for client mechname");
    obj->now = (uint8_t *)sasl->mechname;
    obj->end = obj->now + sasl->len;
    obj->done = reds_handle_auth_mechname;
    async_read_handler(0, 0, &link->async_read);
}
