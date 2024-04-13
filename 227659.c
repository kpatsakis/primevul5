static int ca8210_register_ext_clock(struct spi_device *spi)
{
	struct device_node *np = spi->dev.of_node;
	struct ca8210_priv *priv = spi_get_drvdata(spi);
	struct ca8210_platform_data *pdata = spi->dev.platform_data;
	int ret = 0;

	if (!np)
		return -EFAULT;

	priv->clk = clk_register_fixed_rate(
		&spi->dev,
		np->name,
		NULL,
		0,
		pdata->extclockfreq
	);

	if (IS_ERR(priv->clk)) {
		dev_crit(&spi->dev, "Failed to register external clk\n");
		return PTR_ERR(priv->clk);
	}
	ret = of_clk_add_provider(np, of_clk_src_simple_get, priv->clk);
	if (ret) {
		clk_unregister(priv->clk);
		dev_crit(
			&spi->dev,
			"Failed to register external clock as clock provider\n"
		);
	} else {
		dev_info(&spi->dev, "External clock set as clock provider\n");
	}

	return ret;
}