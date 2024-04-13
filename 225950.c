static int ms_power_on(struct rtsx_usb_ms *host)
{
	struct rtsx_ucr *ucr = host->ucr;
	int err;

	dev_dbg(ms_dev(host), "%s\n", __func__);

	rtsx_usb_init_cmd(ucr);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_SELECT, 0x07, MS_MOD_SEL);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_SHARE_MODE,
			CARD_SHARE_MASK, CARD_SHARE_MS);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_CLK_EN,
			MS_CLK_EN, MS_CLK_EN);
	err = rtsx_usb_send_cmd(ucr, MODE_C, 100);
	if (err < 0)
		return err;

	if (CHECK_PKG(ucr, LQFP48))
		err = ms_pull_ctl_enable_lqfp48(ucr);
	else
		err = ms_pull_ctl_enable_qfn24(ucr);
	if (err < 0)
		return err;

	err = rtsx_usb_write_register(ucr, CARD_PWR_CTL,
			POWER_MASK, PARTIAL_POWER_ON);
	if (err)
		return err;

	usleep_range(800, 1000);

	rtsx_usb_init_cmd(ucr);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_PWR_CTL,
			POWER_MASK, POWER_ON);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_OE,
			MS_OUTPUT_EN, MS_OUTPUT_EN);

	return rtsx_usb_send_cmd(ucr, MODE_C, 100);
}