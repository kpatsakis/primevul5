static void ca8210_unregister_ext_clock(struct spi_device *spi)
{
	struct ca8210_priv *priv = spi_get_drvdata(spi);

	if (!priv->clk)
		return

	of_clk_del_provider(spi->dev.of_node);
	clk_unregister(priv->clk);
	dev_info(&spi->dev, "External clock unregistered\n");
}