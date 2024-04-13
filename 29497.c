static void reds_get_spice_ticket(RedLinkInfo *link)
{
    AsyncRead *obj = &link->async_read;

    obj->now = (uint8_t *)&link->tiTicketing.encrypted_ticket.encrypted_data;
    obj->end = obj->now + link->tiTicketing.rsa_size;
    obj->done = reds_handle_ticket;
    async_read_handler(0, 0, &link->async_read);
}
