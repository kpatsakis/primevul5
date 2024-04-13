static int ca8210_reset_init(struct spi_device *spi)
{
	int ret;
	struct ca8210_platform_data *pdata = spi->dev.platform_data;

	pdata->gpio_reset = of_get_named_gpio(
		spi->dev.of_node,
		"reset-gpio",
		0
	);

	ret = gpio_direction_output(pdata->gpio_reset, 1);
	if (ret < 0) {
		dev_crit(
			&spi->dev,
			"Reset GPIO %d did not set to output mode\n",
			pdata->gpio_reset
		);
	}

	return ret;
}