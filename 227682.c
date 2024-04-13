static void ca8210_mlme_reset_worker(struct work_struct *work)
{
	struct work_priv_container *wpc = container_of(
		work,
		struct work_priv_container,
		work
	);
	struct ca8210_priv *priv = wpc->priv;

	mlme_reset_request_sync(0, priv->spi);
	kfree(wpc);
}