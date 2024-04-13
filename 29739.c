static void red_channel_remove_client(RedChannelClient *rcc)
{
    if (!pthread_equal(pthread_self(), rcc->channel->thread_id)) {
        spice_warning("channel type %d id %d - "
                      "channel->thread_id (0x%lx) != pthread_self (0x%lx)."
                      "If one of the threads is != io-thread && != vcpu-thread, "
                      "this might be a BUG",
                      rcc->channel->type, rcc->channel->id,
                      rcc->channel->thread_id, pthread_self());
    }
    ring_remove(&rcc->channel_link);
    spice_assert(rcc->channel->clients_num > 0);
    rcc->channel->clients_num--;
}
