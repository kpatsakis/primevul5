static void reds_stream_push_channel_event(RedsStream *s, int event)
{
    main_dispatcher_channel_event(event, s->info);
}
