void reds_register_channel(RedChannel *channel)
{
    spice_assert(reds);
    ring_add(&reds->channels, &channel->link);
    reds->num_of_channels++;
}
