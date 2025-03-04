static int amd_gpio_probe(struct platform_device *pdev)
{
	int ret = 0;
	int irq_base;
	struct resource *res;
	struct amd_gpio *gpio_dev;

	gpio_dev = devm_kzalloc(&pdev->dev,
				sizeof(struct amd_gpio), GFP_KERNEL);
	if (!gpio_dev)
		return -ENOMEM;

	spin_lock_init(&gpio_dev->lock);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "Failed to get gpio io resource.\n");
		return -EINVAL;
	}

	gpio_dev->base = devm_ioremap_nocache(&pdev->dev, res->start,
						resource_size(res));
	if (!gpio_dev->base)
		return -ENOMEM;

	irq_base = platform_get_irq(pdev, 0);
	if (irq_base < 0) {
		dev_err(&pdev->dev, "Failed to get gpio IRQ.\n");
		return -EINVAL;
	}

	gpio_dev->pdev = pdev;
	gpio_dev->gc.direction_input	= amd_gpio_direction_input;
	gpio_dev->gc.direction_output	= amd_gpio_direction_output;
	gpio_dev->gc.get			= amd_gpio_get_value;
	gpio_dev->gc.set			= amd_gpio_set_value;
	gpio_dev->gc.set_debounce	= amd_gpio_set_debounce;
	gpio_dev->gc.dbg_show		= amd_gpio_dbg_show;

	gpio_dev->gc.base		= -1;
	gpio_dev->gc.label			= pdev->name;
	gpio_dev->gc.owner			= THIS_MODULE;
	gpio_dev->gc.parent			= &pdev->dev;
	gpio_dev->gc.ngpio			= resource_size(res) / 4;
#if defined(CONFIG_OF_GPIO)
	gpio_dev->gc.of_node			= pdev->dev.of_node;
#endif

	gpio_dev->hwbank_num = gpio_dev->gc.ngpio / 64;
	gpio_dev->groups = kerncz_groups;
	gpio_dev->ngroups = ARRAY_SIZE(kerncz_groups);

	amd_pinctrl_desc.name = dev_name(&pdev->dev);
	gpio_dev->pctrl = devm_pinctrl_register(&pdev->dev, &amd_pinctrl_desc,
						gpio_dev);
	if (IS_ERR(gpio_dev->pctrl)) {
		dev_err(&pdev->dev, "Couldn't register pinctrl driver\n");
		return PTR_ERR(gpio_dev->pctrl);
	}

	ret = gpiochip_add_data(&gpio_dev->gc, gpio_dev);
	if (ret)
		return ret;

	ret = gpiochip_add_pin_range(&gpio_dev->gc, dev_name(&pdev->dev),
				0, 0, gpio_dev->gc.ngpio);
	if (ret) {
		dev_err(&pdev->dev, "Failed to add pin range\n");
		goto out2;
	}

	ret = gpiochip_irqchip_add(&gpio_dev->gc,
				&amd_gpio_irqchip,
				0,
				handle_simple_irq,
				IRQ_TYPE_NONE);
	if (ret) {
		dev_err(&pdev->dev, "could not add irqchip\n");
		ret = -ENODEV;
		goto out2;
	}

	gpiochip_set_chained_irqchip(&gpio_dev->gc,
				 &amd_gpio_irqchip,
				 irq_base,
				 amd_gpio_irq_handler);
	platform_set_drvdata(pdev, gpio_dev);

	dev_dbg(&pdev->dev, "amd gpio driver loaded\n");
	return ret;

out2:
	gpiochip_remove(&gpio_dev->gc);

	return ret;
}