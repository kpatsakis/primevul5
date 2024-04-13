static int ms_transfer_data(struct rtsx_usb_ms *host, unsigned char data_dir,
		u8 tpc, u8 cfg, struct scatterlist *sg)
{
	struct rtsx_ucr *ucr = host->ucr;
	int err;
	unsigned int length = sg->length;
	u16 sec_cnt = (u16)(length / 512);
	u8 trans_mode, dma_dir, flag;
	unsigned int pipe;
	struct memstick_dev *card = host->msh->card;

	dev_dbg(ms_dev(host), "%s: tpc = 0x%02x, data_dir = %s, length = %d\n",
			__func__, tpc, (data_dir == READ) ? "READ" : "WRITE",
			length);

	if (data_dir == READ) {
		flag = MODE_CDIR;
		dma_dir = DMA_DIR_FROM_CARD;
		if (card->id.type != MEMSTICK_TYPE_PRO)
			trans_mode = MS_TM_NORMAL_READ;
		else
			trans_mode = MS_TM_AUTO_READ;
		pipe = usb_rcvbulkpipe(ucr->pusb_dev, EP_BULK_IN);
	} else {
		flag = MODE_CDOR;
		dma_dir = DMA_DIR_TO_CARD;
		if (card->id.type != MEMSTICK_TYPE_PRO)
			trans_mode = MS_TM_NORMAL_WRITE;
		else
			trans_mode = MS_TM_AUTO_WRITE;
		pipe = usb_sndbulkpipe(ucr->pusb_dev, EP_BULK_OUT);
	}

	rtsx_usb_init_cmd(ucr);

	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_TPC, 0xFF, tpc);
	if (card->id.type == MEMSTICK_TYPE_PRO) {
		rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_SECTOR_CNT_H,
				0xFF, (u8)(sec_cnt >> 8));
		rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_SECTOR_CNT_L,
				0xFF, (u8)sec_cnt);
	}
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_TRANS_CFG, 0xFF, cfg);

	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MC_DMA_TC3,
			0xFF, (u8)(length >> 24));
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MC_DMA_TC2,
			0xFF, (u8)(length >> 16));
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MC_DMA_TC1,
			0xFF, (u8)(length >> 8));
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MC_DMA_TC0, 0xFF,
			(u8)length);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MC_DMA_CTL,
			0x03 | DMA_PACK_SIZE_MASK, dma_dir | DMA_EN | DMA_512);
	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, CARD_DATA_SOURCE,
			0x01, RING_BUFFER);

	rtsx_usb_add_cmd(ucr, WRITE_REG_CMD, MS_TRANSFER,
			0xFF, MS_TRANSFER_START | trans_mode);
	rtsx_usb_add_cmd(ucr, CHECK_REG_CMD, MS_TRANSFER,
			MS_TRANSFER_END, MS_TRANSFER_END);

	err = rtsx_usb_send_cmd(ucr, flag | STAGE_MS_STATUS, 100);
	if (err)
		return err;

	err = rtsx_usb_transfer_data(ucr, pipe, sg, length,
			1, NULL, 10000);
	if (err)
		goto err_out;

	err = rtsx_usb_get_rsp(ucr, 3, 15000);
	if (err)
		goto err_out;

	if (ucr->rsp_buf[0] & MS_TRANSFER_ERR ||
	    ucr->rsp_buf[1] & (MS_CRC16_ERR | MS_RDY_TIMEOUT)) {
		err = -EIO;
		goto err_out;
	}
	return 0;
err_out:
	ms_clear_error(host);
	return err;
}