static void rtsx_usb_ms_handle_req(struct work_struct *work)
{
	struct rtsx_usb_ms *host = container_of(work,
			struct rtsx_usb_ms, handle_req);
	struct rtsx_ucr *ucr = host->ucr;
	struct memstick_host *msh = host->msh;
	int rc;

	if (!host->req) {
		pm_runtime_get_sync(ms_dev(host));
		do {
			rc = memstick_next_req(msh, &host->req);
			dev_dbg(ms_dev(host), "next req %d\n", rc);

			if (!rc) {
				mutex_lock(&ucr->dev_mutex);

				if (rtsx_usb_card_exclusive_check(ucr,
							RTSX_USB_MS_CARD))
					host->req->error = -EIO;
				else
					host->req->error =
						rtsx_usb_ms_issue_cmd(host);

				mutex_unlock(&ucr->dev_mutex);

				dev_dbg(ms_dev(host), "req result %d\n",
						host->req->error);
			}
		} while (!rc);
		pm_runtime_put_sync(ms_dev(host));
	}

}