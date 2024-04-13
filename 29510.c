static void reds_handle_new_link(RedLinkInfo *link)
{
    AsyncRead *obj = &link->async_read;
    obj->opaque = link;
    obj->stream = link->stream;
    obj->now = (uint8_t *)&link->link_header;
    obj->end = (uint8_t *)((SpiceLinkHeader *)&link->link_header + 1);
    obj->done = reds_handle_read_header_done;
    obj->error = reds_handle_link_error;
    async_read_handler(0, 0, &link->async_read);
}
