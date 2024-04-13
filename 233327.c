static void amd_gpio_dbg_show(struct seq_file *s, struct gpio_chip *gc)
{
	u32 pin_reg;
	unsigned long flags;
	unsigned int bank, i, pin_num;
	struct amd_gpio *gpio_dev = gpiochip_get_data(gc);

	char *level_trig;
	char *active_level;
	char *interrupt_enable;
	char *interrupt_mask;
	char *wake_cntrl0;
	char *wake_cntrl1;
	char *wake_cntrl2;
	char *pin_sts;
	char *pull_up_sel;
	char *pull_up_enable;
	char *pull_down_enable;
	char *output_value;
	char *output_enable;

	for (bank = 0; bank < gpio_dev->hwbank_num; bank++) {
		seq_printf(s, "GPIO bank%d\t", bank);

		switch (bank) {
		case 0:
			i = 0;
			pin_num = AMD_GPIO_PINS_BANK0;
			break;
		case 1:
			i = 64;
			pin_num = AMD_GPIO_PINS_BANK1 + i;
			break;
		case 2:
			i = 128;
			pin_num = AMD_GPIO_PINS_BANK2 + i;
			break;
		case 3:
			i = 192;
			pin_num = AMD_GPIO_PINS_BANK3 + i;
			break;
		}
		for (; i < pin_num; i++) {
			seq_printf(s, "pin%d\t", i);
			spin_lock_irqsave(&gpio_dev->lock, flags);
			pin_reg = readl(gpio_dev->base + i * 4);
			spin_unlock_irqrestore(&gpio_dev->lock, flags);

			if (pin_reg & BIT(INTERRUPT_ENABLE_OFF)) {
				interrupt_enable = "interrupt is enabled|";

		if (!(pin_reg & BIT(ACTIVE_LEVEL_OFF))
				&& !(pin_reg & BIT(ACTIVE_LEVEL_OFF+1)))
					active_level = "Active low|";
				else if (pin_reg & BIT(ACTIVE_LEVEL_OFF)
				&& !(pin_reg & BIT(ACTIVE_LEVEL_OFF+1)))
					active_level = "Active high|";
				else if (!(pin_reg & BIT(ACTIVE_LEVEL_OFF))
					&& pin_reg & BIT(ACTIVE_LEVEL_OFF+1))
					active_level = "Active on both|";
				else
					active_level = "Unknow Active level|";

				if (pin_reg & BIT(LEVEL_TRIG_OFF))
					level_trig = "Level trigger|";
				else
					level_trig = "Edge trigger|";

			} else {
				interrupt_enable =
					"interrupt is disabled|";
				active_level = " ";
				level_trig = " ";
			}

			if (pin_reg & BIT(INTERRUPT_MASK_OFF))
				interrupt_mask =
					"interrupt is unmasked|";
			else
				interrupt_mask =
					"interrupt is masked|";

			if (pin_reg & BIT(WAKE_CNTRL_OFF_S0I3))
				wake_cntrl0 = "enable wakeup in S0i3 state|";
			else
				wake_cntrl0 = "disable wakeup in S0i3 state|";

			if (pin_reg & BIT(WAKE_CNTRL_OFF_S3))
				wake_cntrl1 = "enable wakeup in S3 state|";
			else
				wake_cntrl1 = "disable wakeup in S3 state|";

			if (pin_reg & BIT(WAKE_CNTRL_OFF_S4))
				wake_cntrl2 = "enable wakeup in S4/S5 state|";
			else
				wake_cntrl2 = "disable wakeup in S4/S5 state|";

			if (pin_reg & BIT(PULL_UP_ENABLE_OFF)) {
				pull_up_enable = "pull-up is enabled|";
				if (pin_reg & BIT(PULL_UP_SEL_OFF))
					pull_up_sel = "8k pull-up|";
				else
					pull_up_sel = "4k pull-up|";
			} else {
				pull_up_enable = "pull-up is disabled|";
				pull_up_sel = " ";
			}

			if (pin_reg & BIT(PULL_DOWN_ENABLE_OFF))
				pull_down_enable = "pull-down is enabled|";
			else
				pull_down_enable = "Pull-down is disabled|";

			if (pin_reg & BIT(OUTPUT_ENABLE_OFF)) {
				pin_sts = " ";
				output_enable = "output is enabled|";
				if (pin_reg & BIT(OUTPUT_VALUE_OFF))
					output_value = "output is high|";
				else
					output_value = "output is low|";
			} else {
				output_enable = "output is disabled|";
				output_value = " ";

				if (pin_reg & BIT(PIN_STS_OFF))
					pin_sts = "input is high|";
				else
					pin_sts = "input is low|";
			}

			seq_printf(s, "%s %s %s %s %s %s\n"
				" %s %s %s %s %s %s %s 0x%x\n",
				level_trig, active_level, interrupt_enable,
				interrupt_mask, wake_cntrl0, wake_cntrl1,
				wake_cntrl2, pin_sts, pull_up_sel,
				pull_up_enable, pull_down_enable,
				output_value, output_enable, pin_reg);
		}
	}
}