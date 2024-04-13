
static void io_rsrc_node_switch(struct io_ring_ctx *ctx,
				struct io_rsrc_data *data_to_kill)
{
	WARN_ON_ONCE(!ctx->rsrc_backup_node);
	WARN_ON_ONCE(data_to_kill && !ctx->rsrc_node);

	if (data_to_kill) {
		struct io_rsrc_node *rsrc_node = ctx->rsrc_node;

		rsrc_node->rsrc_data = data_to_kill;
		io_rsrc_ref_lock(ctx);
		list_add_tail(&rsrc_node->node, &ctx->rsrc_ref_list);
		io_rsrc_ref_unlock(ctx);

		atomic_inc(&data_to_kill->refs);
		percpu_ref_kill(&rsrc_node->refs);
		ctx->rsrc_node = NULL;
	}

	if (!ctx->rsrc_node) {
		ctx->rsrc_node = ctx->rsrc_backup_node;
		ctx->rsrc_backup_node = NULL;
	}