static void ca8210_dev_com_clear(struct ca8210_priv *priv)
{
	flush_workqueue(priv->mlme_workqueue);
	destroy_workqueue(priv->mlme_workqueue);
	flush_workqueue(priv->irq_workqueue);
	destroy_workqueue(priv->irq_workqueue);
}