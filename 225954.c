static void rtsx_usb_ms_poll_card(struct work_struct *work)
{
	struct rtsx_usb_ms *host = container_of(work, struct rtsx_usb_ms,
			poll_card.work);
	struct rtsx_ucr *ucr = host->ucr;
	int err;
	u8 val;

	if (host->eject || host->power_mode != MEMSTICK_POWER_ON)
		return;

	pm_runtime_get_sync(ms_dev(host));
	mutex_lock(&ucr->dev_mutex);

	/* Check pending MS card changes */
	err = rtsx_usb_read_register(ucr, CARD_INT_PEND, &val);
	if (err) {
		mutex_unlock(&ucr->dev_mutex);
		goto poll_again;
	}

	/* Clear the pending */
	rtsx_usb_write_register(ucr, CARD_INT_PEND,
			XD_INT | MS_INT | SD_INT,
			XD_INT | MS_INT | SD_INT);

	mutex_unlock(&ucr->dev_mutex);

	if (val & MS_INT) {
		dev_dbg(ms_dev(host), "MS slot change detected\n");
		memstick_detect_change(host->msh);
	}

poll_again:
	pm_runtime_put_sync(ms_dev(host));

	if (!host->eject && host->power_mode == MEMSTICK_POWER_ON)
		schedule_delayed_work(&host->poll_card, 100);
}