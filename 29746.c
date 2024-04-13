static void red_channel_unref(RedChannel *channel)
{
    if (!--channel->refs) {
        if (channel->local_caps.num_common_caps) {
            free(channel->local_caps.common_caps);
        }

        if (channel->local_caps.num_caps) {
            free(channel->local_caps.caps);
        }

        free(channel);
    }
}
