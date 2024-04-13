 */
static void bfq_limit_depth(unsigned int op, struct blk_mq_alloc_data *data)
{
	struct bfq_data *bfqd = data->q->elevator->elevator_data;

	if (op_is_sync(op) && !op_is_write(op))
		return;

	data->shallow_depth =
		bfqd->word_depths[!!bfqd->wr_busy_queues][op_is_sync(op)];

	bfq_log(bfqd, "[%s] wr_busy %d sync %d depth %u",
			__func__, bfqd->wr_busy_queues, op_is_sync(op),
			data->shallow_depth);