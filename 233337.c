static void amd_gpio_irq_handler(struct irq_desc *desc)
{
	u32 i;
	u32 off;
	u32 reg;
	u32 pin_reg;
	u64 reg64;
	int handled = 0;
	unsigned int irq;
	unsigned long flags;
	struct irq_chip *chip = irq_desc_get_chip(desc);
	struct gpio_chip *gc = irq_desc_get_handler_data(desc);
	struct amd_gpio *gpio_dev = gpiochip_get_data(gc);

	chained_irq_enter(chip, desc);
	/*enable GPIO interrupt again*/
	spin_lock_irqsave(&gpio_dev->lock, flags);
	reg = readl(gpio_dev->base + WAKE_INT_STATUS_REG1);
	reg64 = reg;
	reg64 = reg64 << 32;

	reg = readl(gpio_dev->base + WAKE_INT_STATUS_REG0);
	reg64 |= reg;
	spin_unlock_irqrestore(&gpio_dev->lock, flags);

	/*
	 * first 46 bits indicates interrupt status.
	 * one bit represents four interrupt sources.
	*/
	for (off = 0; off < 46 ; off++) {
		if (reg64 & BIT(off)) {
			for (i = 0; i < 4; i++) {
				pin_reg = readl(gpio_dev->base +
						(off * 4 + i) * 4);
				if ((pin_reg & BIT(INTERRUPT_STS_OFF)) ||
					(pin_reg & BIT(WAKE_STS_OFF))) {
					irq = irq_find_mapping(gc->irqdomain,
								off * 4 + i);
					generic_handle_irq(irq);
					writel(pin_reg,
						gpio_dev->base
						+ (off * 4 + i) * 4);
					handled++;
				}
			}
		}
	}

	if (handled == 0)
		handle_bad_irq(desc);

	spin_lock_irqsave(&gpio_dev->lock, flags);
	reg = readl(gpio_dev->base + WAKE_INT_MASTER_REG);
	reg |= EOI_MASK;
	writel(reg, gpio_dev->base + WAKE_INT_MASTER_REG);
	spin_unlock_irqrestore(&gpio_dev->lock, flags);

	chained_irq_exit(chip, desc);
}