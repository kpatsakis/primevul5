static void io_flush_cached_locked_reqs(struct io_ring_ctx *ctx,
					struct io_comp_state *cs)
{
	spin_lock_irq(&ctx->completion_lock);
	list_splice_init(&cs->locked_free_list, &cs->free_list);
	cs->locked_free_nr = 0;
	spin_unlock_irq(&ctx->completion_lock);
}