static int ms_read_bytes(struct rtsx_usb_ms *host, u8 tpc,
		u8 cfg, u8 cnt, u8 *data, u8 *int_reg)
{
	struct rtsx_ucr *ucr = host->ucr;
	int err, i;
	u8 *ptr;

	dev_dbg(ms_dev(host), "%s: tpc = 0x%02x\n", __func__, tpc);

	rtsx_usb_init_cmd(ucr);

	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_TPC, 0xFF, tpc);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_BYTE_CNT, 0xFF, cnt);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_TRANS_CFG, 0xFF, cfg);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_DATA_SOURCE,
			0x01, PINGPONG_BUFFER);

	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_TRANSFER,
			0xFF, MS_TRANSFER_START | MS_TM_READ_BYTES);
	rtsx_usb_add_cmd(ucr, CHECK_REG_CMD, MS_TRANSFER,
			MS_TRANSFER_END, MS_TRANSFER_END);
	for (i = 0; i < cnt - 1; i++)
		rtsx_usb_add_cmd(ucr, READ_REG_CMD, PPBUF_BASE2 + i, 0, 0);
	if (cnt % 2)
		rtsx_usb_add_cmd(ucr, READ_REG_CMD, PPBUF_BASE2 + cnt, 0, 0);
	else
		rtsx_usb_add_cmd(ucr, READ_REG_CMD,
				PPBUF_BASE2 + cnt - 1, 0, 0);

	rtsx_usb_add_cmd(ucr, READ_REG_CMD, MS_TRANS_CFG, 0, 0);

	err = rtsx_usb_send_cmd(ucr, MODE_CR, 100);
	if (err)
		return err;

	err = rtsx_usb_get_rsp(ucr, cnt + 2, 5000);
	if (err || (ucr->rsp_buf[0] & MS_TRANSFER_ERR)) {
		u8 val;

		rtsx_usb_ep0_read_register(ucr, MS_TRANS_CFG, &val);
		dev_dbg(ms_dev(host), "MS_TRANS_CFG: 0x%02x\n", val);

		if (int_reg && (host->ifmode != MEMSTICK_SERIAL))
			*int_reg = val & 0x0F;

		ms_print_debug_regs(host);

		ms_clear_error(host);

		if (!(tpc & 0x08)) {
			if (val & MS_CRC16_ERR)
				return -EIO;
		} else {
			if (!(val & 0x80)) {
				if (val & (MS_INT_ERR | MS_INT_CMDNK))
					return -EIO;
			}
		}

		return -ETIMEDOUT;
	}

	ptr = ucr->rsp_buf + 1;
	for (i = 0; i < cnt; i++)
		data[i] = *ptr++;


	if (int_reg && (host->ifmode != MEMSTICK_SERIAL))
		*int_reg = *ptr & 0x0F;

	return 0;
}