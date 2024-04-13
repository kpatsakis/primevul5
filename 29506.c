void reds_handle_channel_event(int event, SpiceChannelEventInfo *info)
{
    if (core->base.minor_version >= 3 && core->channel_event != NULL)
        core->channel_event(event, info);

    if (event == SPICE_CHANNEL_EVENT_DISCONNECTED) {
        free(info);
    }
}
