static int rtsx_usb_ms_runtime_resume(struct device *dev)
{
	struct rtsx_usb_ms *host = dev_get_drvdata(dev);


	if (host->system_suspending)
		return 0;

	memstick_detect_change(host->msh);

	return 0;
}