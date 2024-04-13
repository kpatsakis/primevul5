void red_channel_set_cap(RedChannel *channel, uint32_t cap)
{
    add_capability(&channel->local_caps.caps, &channel->local_caps.num_caps, cap);
}
