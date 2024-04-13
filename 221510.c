
static void io_rsrc_put_work(struct work_struct *work)
{
	struct io_ring_ctx *ctx;
	struct llist_node *node;

	ctx = container_of(work, struct io_ring_ctx, rsrc_put_work.work);
	node = llist_del_all(&ctx->rsrc_put_llist);

	while (node) {
		struct io_rsrc_node *ref_node;
		struct llist_node *next = node->next;

		ref_node = llist_entry(node, struct io_rsrc_node, llist);
		__io_rsrc_put_work(ref_node);
		node = next;
	}