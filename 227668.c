static void ca8210_reset_send(struct spi_device *spi, unsigned int ms)
{
	struct ca8210_platform_data *pdata = spi->dev.platform_data;
	struct ca8210_priv *priv = spi_get_drvdata(spi);
	long status;

	gpio_set_value(pdata->gpio_reset, 0);
	reinit_completion(&priv->ca8210_is_awake);
	msleep(ms);
	gpio_set_value(pdata->gpio_reset, 1);
	priv->promiscuous = false;

	/* Wait until wakeup indication seen */
	status = wait_for_completion_interruptible_timeout(
		&priv->ca8210_is_awake,
		msecs_to_jiffies(CA8210_SYNC_TIMEOUT)
	);
	if (status == 0) {
		dev_crit(
			&spi->dev,
			"Fatal: No wakeup from ca8210 after reset!\n"
		);
	}

	dev_dbg(&spi->dev, "Reset the device\n");
}