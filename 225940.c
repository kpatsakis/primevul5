static void ms_print_debug_regs(struct rtsx_usb_ms *host)
{
	struct rtsx_ucr *ucr = host->ucr;
	u16 i;
	u8 *ptr;

	/* Print MS host internal registers */
	rtsx_usb_init_cmd(ucr);

	/* MS_CFG to MS_INT_REG */
	for (i = 0xFD40; i <= 0xFD44; i++)
		rtsx_usb_add_cmd(ucr, READ_REG_CMD, i, 0, 0);

	/* CARD_SHARE_MODE to CARD_GPIO */
	for (i = 0xFD51; i <= 0xFD56; i++)
		rtsx_usb_add_cmd(ucr, READ_REG_CMD, i, 0, 0);

	/* CARD_PULL_CTLx */
	for (i = 0xFD60; i <= 0xFD65; i++)
		rtsx_usb_add_cmd(ucr, READ_REG_CMD, i, 0, 0);

	/* CARD_DATA_SOURCE, CARD_SELECT, CARD_CLK_EN, CARD_PWR_CTL */
	rtsx_usb_add_cmd(ucr, READ_REG_CMD, CARD_DATA_SOURCE, 0, 0);
	rtsx_usb_add_cmd(ucr, READ_REG_CMD, CARD_SELECT, 0, 0);
	rtsx_usb_add_cmd(ucr, READ_REG_CMD, CARD_CLK_EN, 0, 0);
	rtsx_usb_add_cmd(ucr, READ_REG_CMD, CARD_PWR_CTL, 0, 0);

	rtsx_usb_send_cmd(ucr, MODE_CR, 100);
	rtsx_usb_get_rsp(ucr, 21, 100);

	ptr = ucr->rsp_buf;
	for (i = 0xFD40; i <= 0xFD44; i++)
		dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", i, *(ptr++));
	for (i = 0xFD51; i <= 0xFD56; i++)
		dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", i, *(ptr++));
	for (i = 0xFD60; i <= 0xFD65; i++)
		dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", i, *(ptr++));

	dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", CARD_DATA_SOURCE, *(ptr++));
	dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", CARD_SELECT, *(ptr++));
	dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", CARD_CLK_EN, *(ptr++));
	dev_dbg(ms_dev(host), "0x%04X: 0x%02x\n", CARD_PWR_CTL, *(ptr++));
}