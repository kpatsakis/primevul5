
static void __io_rsrc_put_work(struct io_rsrc_node *ref_node)
{
	struct io_rsrc_data *rsrc_data = ref_node->rsrc_data;
	struct io_ring_ctx *ctx = rsrc_data->ctx;
	struct io_rsrc_put *prsrc, *tmp;

	list_for_each_entry_safe(prsrc, tmp, &ref_node->rsrc_list, list) {
		list_del(&prsrc->list);

		if (prsrc->tag) {
			bool lock_ring = ctx->flags & IORING_SETUP_IOPOLL;
			unsigned long flags;

			io_ring_submit_lock(ctx, lock_ring);
			spin_lock_irqsave(&ctx->completion_lock, flags);
			io_cqring_fill_event(ctx, prsrc->tag, 0, 0);
			ctx->cq_extra++;
			io_commit_cqring(ctx);
			spin_unlock_irqrestore(&ctx->completion_lock, flags);
			io_cqring_ev_posted(ctx);
			io_ring_submit_unlock(ctx, lock_ring);
		}

		rsrc_data->do_put(ctx, prsrc);
		kfree(prsrc);
	}

	io_rsrc_node_destroy(ref_node);
	if (atomic_dec_and_test(&rsrc_data->refs))
		complete(&rsrc_data->done);