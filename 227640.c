static int ca8210_get_platform_data(
	struct spi_device *spi_device,
	struct ca8210_platform_data *pdata
)
{
	int ret = 0;

	if (!spi_device->dev.of_node)
		return -EINVAL;

	pdata->extclockenable = of_property_read_bool(
		spi_device->dev.of_node,
		"extclock-enable"
	);
	if (pdata->extclockenable) {
		ret = of_property_read_u32(
			spi_device->dev.of_node,
			"extclock-freq",
			&pdata->extclockfreq
		);
		if (ret < 0)
			return ret;

		ret = of_property_read_u32(
			spi_device->dev.of_node,
			"extclock-gpio",
			&pdata->extclockgpio
		);
	}

	return ret;
}