
static void bfq_depth_updated(struct blk_mq_hw_ctx *hctx)
{
	struct bfq_data *bfqd = hctx->queue->elevator->elevator_data;
	struct blk_mq_tags *tags = hctx->sched_tags;
	unsigned int min_shallow;

	min_shallow = bfq_update_depths(bfqd, &tags->bitmap_tags);
	sbitmap_queue_min_shallow_depth(&tags->bitmap_tags, min_shallow);