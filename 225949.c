static inline void ms_clear_error(struct rtsx_usb_ms *host)
{
	struct rtsx_ucr *ucr = host->ucr;
	rtsx_usb_ep0_write_register(ucr, CARD_STOP,
				  MS_STOP | MS_CLR_ERR,
				  MS_STOP | MS_CLR_ERR);

	rtsx_usb_clear_dma_err(ucr);
	rtsx_usb_clear_fsm_err(ucr);
}