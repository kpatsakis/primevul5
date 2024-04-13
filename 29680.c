static void red_channel_client_on_output(void *opaque, int n)
{
    RedChannelClient *rcc = opaque;

    stat_inc_counter(rcc->channel->out_bytes_counter, n);
}
