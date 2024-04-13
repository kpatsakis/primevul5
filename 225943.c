static int rtsx_usb_ms_issue_cmd(struct rtsx_usb_ms *host)
{
	struct memstick_request *req = host->req;
	int err = 0;
	u8 cfg = 0, int_reg;

	dev_dbg(ms_dev(host), "%s\n", __func__);

	if (req->need_card_int) {
		if (host->ifmode != MEMSTICK_SERIAL)
			cfg = WAIT_INT;
	}

	if (req->long_data) {
		err = ms_transfer_data(host, req->data_dir,
				req->tpc, cfg, &(req->sg));
	} else {
		if (req->data_dir == READ)
			err = ms_read_bytes(host, req->tpc, cfg,
					req->data_len, req->data, &int_reg);
		else
			err = ms_write_bytes(host, req->tpc, cfg,
					req->data_len, req->data, &int_reg);
	}
	if (err < 0)
		return err;

	if (req->need_card_int) {
		if (host->ifmode == MEMSTICK_SERIAL) {
			err = ms_read_bytes(host, MS_TPC_GET_INT,
					NO_WAIT_INT, 1, &req->int_reg, NULL);
			if (err < 0)
				return err;
		} else {

			if (int_reg & MS_INT_CMDNK)
				req->int_reg |= MEMSTICK_INT_CMDNAK;
			if (int_reg & MS_INT_BREQ)
				req->int_reg |= MEMSTICK_INT_BREQ;
			if (int_reg & MS_INT_ERR)
				req->int_reg |= MEMSTICK_INT_ERR;
			if (int_reg & MS_INT_CED)
				req->int_reg |= MEMSTICK_INT_CED;
		}
		dev_dbg(ms_dev(host), "int_reg: 0x%02x\n", req->int_reg);
	}

	return 0;
}