int red_channel_client_test_remote_cap(RedChannelClient *rcc, uint32_t cap)
{
    return test_capabilty(rcc->remote_caps.caps,
                          rcc->remote_caps.num_caps,
                          cap);
}
