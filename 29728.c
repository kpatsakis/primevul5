static void red_channel_handle_migrate_flush_mark(RedChannelClient *rcc)
{
    if (rcc->channel->channel_cbs.handle_migrate_flush_mark) {
        rcc->channel->channel_cbs.handle_migrate_flush_mark(rcc);
    }
}
