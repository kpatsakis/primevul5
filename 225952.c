static int ms_power_off(struct rtsx_usb_ms *host)
{
	struct rtsx_ucr *ucr = host->ucr;
	int err;

	dev_dbg(ms_dev(host), "%s\n", __func__);

	rtsx_usb_init_cmd(ucr);

	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_CLK_EN, MS_CLK_EN, 0);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_OE, MS_OUTPUT_EN, 0);

	err = rtsx_usb_send_cmd(ucr, MODE_C, 100);
	if (err < 0)
		return err;

	if (CHECK_PKG(ucr, LQFP48))
		return ms_pull_ctl_disable_lqfp48(ucr);

	return ms_pull_ctl_disable_qfn24(ucr);
}