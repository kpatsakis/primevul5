
static int bfq_init_hctx(struct blk_mq_hw_ctx *hctx, unsigned int index)
{
	bfq_depth_updated(hctx);
	return 0;