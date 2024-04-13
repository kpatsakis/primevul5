static void reds_send_link_result(RedLinkInfo *link, uint32_t error)
{
    sync_write(link->stream, &error, sizeof(error));
}
