static int ca8210_interrupt_init(struct spi_device *spi)
{
	int ret;
	struct ca8210_platform_data *pdata = spi->dev.platform_data;

	pdata->gpio_irq = of_get_named_gpio(
		spi->dev.of_node,
		"irq-gpio",
		0
	);

	pdata->irq_id = gpio_to_irq(pdata->gpio_irq);
	if (pdata->irq_id < 0) {
		dev_crit(
			&spi->dev,
			"Could not get irq for gpio pin %d\n",
			pdata->gpio_irq
		);
		gpio_free(pdata->gpio_irq);
		return pdata->irq_id;
	}

	ret = request_irq(
		pdata->irq_id,
		ca8210_interrupt_handler,
		IRQF_TRIGGER_FALLING,
		"ca8210-irq",
		spi_get_drvdata(spi)
	);
	if (ret) {
		dev_crit(&spi->dev, "request_irq %d failed\n", pdata->irq_id);
		gpio_unexport(pdata->gpio_irq);
		gpio_free(pdata->gpio_irq);
	}

	return ret;
}