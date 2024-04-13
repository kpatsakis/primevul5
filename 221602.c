
static int io_rsrc_ref_quiesce(struct io_rsrc_data *data, struct io_ring_ctx *ctx)
{
	int ret;

	/* As we may drop ->uring_lock, other task may have started quiesce */
	if (data->quiesce)
		return -ENXIO;

	data->quiesce = true;
	do {
		ret = io_rsrc_node_switch_start(ctx);
		if (ret)
			break;
		io_rsrc_node_switch(ctx, data);

		/* kill initial ref, already quiesced if zero */
		if (atomic_dec_and_test(&data->refs))
			break;
		flush_delayed_work(&ctx->rsrc_put_work);
		ret = wait_for_completion_interruptible(&data->done);
		if (!ret)
			break;

		atomic_inc(&data->refs);
		/* wait for all works potentially completing data->done */
		flush_delayed_work(&ctx->rsrc_put_work);
		reinit_completion(&data->done);

		mutex_unlock(&ctx->uring_lock);
		ret = io_run_task_work_sig();
		mutex_lock(&ctx->uring_lock);
	} while (ret >= 0);
	data->quiesce = false;

	return ret;