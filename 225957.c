static void rtsx_usb_ms_request(struct memstick_host *msh)
{
	struct rtsx_usb_ms *host = memstick_priv(msh);

	dev_dbg(ms_dev(host), "--> %s\n", __func__);

	if (!host->eject)
		schedule_work(&host->handle_req);
}