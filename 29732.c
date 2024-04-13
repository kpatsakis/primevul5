void red_channel_pipe_item_init(RedChannel *channel, PipeItem *item, int type)
{
    ring_item_init(&item->link);
    item->type = type;
}
