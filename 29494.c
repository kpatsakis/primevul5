static RedClient *reds_get_client(void)
{
    spice_assert(reds->num_clients <= 1);

    if (reds->num_clients == 0) {
        return NULL;
    }

    return SPICE_CONTAINEROF(ring_get_head(&reds->clients), RedClient, link);
}
