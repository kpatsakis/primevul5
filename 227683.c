static irqreturn_t ca8210_interrupt_handler(int irq, void *dev_id)
{
	struct ca8210_priv *priv = dev_id;
	int status;

	dev_dbg(&priv->spi->dev, "irq: Interrupt occurred\n");
	do {
		status = ca8210_spi_transfer(priv->spi, NULL, 0);
		if (status && (status != -EBUSY)) {
			dev_warn(
				&priv->spi->dev,
				"spi read failed, returned %d\n",
				status
			);
		}
	} while (status == -EBUSY);
	return IRQ_HANDLED;
}