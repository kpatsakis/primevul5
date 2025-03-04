static int amd_pinconf_set(struct pinctrl_dev *pctldev, unsigned int pin,
				unsigned long *configs, unsigned num_configs)
{
	int i;
	u32 arg;
	int ret = 0;
	u32 pin_reg;
	unsigned long flags;
	enum pin_config_param param;
	struct amd_gpio *gpio_dev = pinctrl_dev_get_drvdata(pctldev);

	spin_lock_irqsave(&gpio_dev->lock, flags);
	for (i = 0; i < num_configs; i++) {
		param = pinconf_to_config_param(configs[i]);
		arg = pinconf_to_config_argument(configs[i]);
		pin_reg = readl(gpio_dev->base + pin*4);

		switch (param) {
		case PIN_CONFIG_INPUT_DEBOUNCE:
			pin_reg &= ~DB_TMR_OUT_MASK;
			pin_reg |= arg & DB_TMR_OUT_MASK;
			break;

		case PIN_CONFIG_BIAS_PULL_DOWN:
			pin_reg &= ~BIT(PULL_DOWN_ENABLE_OFF);
			pin_reg |= (arg & BIT(0)) << PULL_DOWN_ENABLE_OFF;
			break;

		case PIN_CONFIG_BIAS_PULL_UP:
			pin_reg &= ~BIT(PULL_UP_SEL_OFF);
			pin_reg |= (arg & BIT(0)) << PULL_UP_SEL_OFF;
			pin_reg &= ~BIT(PULL_UP_ENABLE_OFF);
			pin_reg |= ((arg>>1) & BIT(0)) << PULL_UP_ENABLE_OFF;
			break;

		case PIN_CONFIG_DRIVE_STRENGTH:
			pin_reg &= ~(DRV_STRENGTH_SEL_MASK
					<< DRV_STRENGTH_SEL_OFF);
			pin_reg |= (arg & DRV_STRENGTH_SEL_MASK)
					<< DRV_STRENGTH_SEL_OFF;
			break;

		default:
			dev_err(&gpio_dev->pdev->dev,
				"Invalid config param %04x\n", param);
			ret = -ENOTSUPP;
		}

		writel(pin_reg, gpio_dev->base + pin*4);
	}
	spin_unlock_irqrestore(&gpio_dev->lock, flags);

	return ret;
}