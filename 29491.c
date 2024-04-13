static int reds_find_client(RedClient *client)
{
    RingItem *item;

    RING_FOREACH(item, &reds->clients) {
        RedClient *list_client;

        list_client = SPICE_CONTAINEROF(item, RedClient, link);
        if (list_client == client) {
            return TRUE;
        }
    }
    return FALSE;
}
