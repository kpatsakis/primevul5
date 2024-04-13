RedChannel *red_channel_create_parser(int size,
                               SpiceCoreInterface *core,
                               uint32_t type, uint32_t id,
                               int handle_acks,
                               spice_parse_channel_func_t parser,
                               channel_handle_parsed_proc handle_parsed,
                               ChannelCbs *channel_cbs,
                               uint32_t migration_flags)
{
    RedChannel *channel = red_channel_create(size, core, type, id,
                                             handle_acks,
                                             do_nothing_handle_message,
                                             channel_cbs,
                                             migration_flags);

    if (channel == NULL) {
        return NULL;
    }
    channel->incoming_cb.handle_parsed = (handle_parsed_proc)handle_parsed;
    channel->incoming_cb.parser = parser;
    return channel;
}
