
static void io_rsrc_node_ref_zero(struct percpu_ref *ref)
{
	struct io_rsrc_node *node = container_of(ref, struct io_rsrc_node, refs);
	struct io_ring_ctx *ctx = node->rsrc_data->ctx;
	bool first_add = false;

	io_rsrc_ref_lock(ctx);
	node->done = true;

	while (!list_empty(&ctx->rsrc_ref_list)) {
		node = list_first_entry(&ctx->rsrc_ref_list,
					    struct io_rsrc_node, node);
		/* recycle ref nodes in order */
		if (!node->done)
			break;
		list_del(&node->node);
		first_add |= llist_add(&node->llist, &ctx->rsrc_put_llist);
	}
	io_rsrc_ref_unlock(ctx);

	if (first_add)
		mod_delayed_work(system_wq, &ctx->rsrc_put_work, HZ);