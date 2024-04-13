static int ca8210_dev_com_init(struct ca8210_priv *priv)
{
	priv->mlme_workqueue = alloc_ordered_workqueue(
		"MLME work queue",
		WQ_UNBOUND
	);
	if (!priv->mlme_workqueue) {
		dev_crit(&priv->spi->dev, "alloc of mlme_workqueue failed!\n");
		return -ENOMEM;
	}

	priv->irq_workqueue = alloc_ordered_workqueue(
		"ca8210 irq worker",
		WQ_UNBOUND
	);
	if (!priv->irq_workqueue) {
		dev_crit(&priv->spi->dev, "alloc of irq_workqueue failed!\n");
		return -ENOMEM;
	}

	return 0;
}