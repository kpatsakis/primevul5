static void red_channel_client_set_remote_caps(RedChannelClient* rcc,
                                               int num_common_caps, uint32_t *common_caps,
                                               int num_caps, uint32_t *caps)
{
    rcc->remote_caps.num_common_caps = num_common_caps;
    rcc->remote_caps.common_caps = spice_memdup(common_caps, num_common_caps * sizeof(uint32_t));

    rcc->remote_caps.num_caps = num_caps;
    rcc->remote_caps.caps = spice_memdup(caps, num_caps * sizeof(uint32_t));
}
