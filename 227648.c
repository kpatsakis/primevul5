static int ca8210_config_extern_clk(
	struct ca8210_platform_data *pdata,
	struct spi_device *spi,
	bool on
)
{
	u8 clkparam[2];

	if (on) {
		dev_info(&spi->dev, "Switching external clock on\n");
		switch (pdata->extclockfreq) {
		case SIXTEEN_MHZ:
			clkparam[0] = 1;
			break;
		case EIGHT_MHZ:
			clkparam[0] = 2;
			break;
		case FOUR_MHZ:
			clkparam[0] = 3;
			break;
		case TWO_MHZ:
			clkparam[0] = 4;
			break;
		case ONE_MHZ:
			clkparam[0] = 5;
			break;
		default:
			dev_crit(&spi->dev, "Invalid extclock-freq\n");
			return -EINVAL;
		}
		clkparam[1] = pdata->extclockgpio;
	} else {
		dev_info(&spi->dev, "Switching external clock off\n");
		clkparam[0] = 0; /* off */
		clkparam[1] = 0;
	}
	return link_to_linux_err(
		hwme_set_request_sync(HWME_SYSCLKOUT, 2, clkparam, spi)
	);
}