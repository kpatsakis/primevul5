static int ca8210_remove(struct spi_device *spi_device)
{
	struct ca8210_priv *priv;
	struct ca8210_platform_data *pdata;

	dev_info(&spi_device->dev, "Removing ca8210\n");

	pdata = spi_device->dev.platform_data;
	if (pdata) {
		if (pdata->extclockenable) {
			ca8210_unregister_ext_clock(spi_device);
			ca8210_config_extern_clk(pdata, spi_device, 0);
		}
		free_irq(pdata->irq_id, spi_device->dev.driver_data);
		kfree(pdata);
		spi_device->dev.platform_data = NULL;
	}
	/* get spi_device private data */
	priv = spi_get_drvdata(spi_device);
	if (priv) {
		dev_info(
			&spi_device->dev,
			"sync_down = %d, sync_up = %d\n",
			priv->sync_down,
			priv->sync_up
		);
		ca8210_dev_com_clear(spi_device->dev.driver_data);
		if (priv->hw) {
			if (priv->hw_registered)
				ieee802154_unregister_hw(priv->hw);
			ieee802154_free_hw(priv->hw);
			priv->hw = NULL;
			dev_info(
				&spi_device->dev,
				"Unregistered & freed ieee802154_hw.\n"
			);
		}
		if (IS_ENABLED(CONFIG_IEEE802154_CA8210_DEBUGFS))
			ca8210_test_interface_clear(priv);
	}

	return 0;
}