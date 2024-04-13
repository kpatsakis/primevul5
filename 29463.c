static int channel_is_secondary(RedChannel *channel)
{
    int i;
    for (i = 0 ; i < sizeof(secondary_channels)/sizeof(secondary_channels[0]); ++i) {
        if (channel->type == secondary_channels[i]) {
            return TRUE;
        }
    }
    return FALSE;
}
