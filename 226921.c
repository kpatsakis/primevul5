static inline void mcba_usb_free_ctx(struct mcba_usb_ctx *ctx)
{
	/* Increase number of free ctxs before freeing ctx */
	atomic_inc(&ctx->priv->free_ctx_cnt);

	ctx->ndx = MCBA_CTX_FREE;

	/* Wake up the queue once ctx is marked free */
	netif_wake_queue(ctx->priv->netdev);
}