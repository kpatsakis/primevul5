static void reds_disconnect(void)
{
    RingItem *link, *next;

    spice_info(NULL);
    RING_FOREACH_SAFE(link, next, &reds->clients) {
        reds_client_disconnect(SPICE_CONTAINEROF(link, RedClient, link));
    }
    reds_mig_cleanup();
}
