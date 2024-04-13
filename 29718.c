int red_channel_client_test_remote_common_cap(RedChannelClient *rcc, uint32_t cap)
{
    return test_capabilty(rcc->remote_caps.common_caps,
                          rcc->remote_caps.num_common_caps,
                          cap);
}
