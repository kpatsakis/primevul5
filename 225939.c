static int rtsx_usb_ms_resume(struct device *dev)
{
	struct rtsx_usb_ms *host = dev_get_drvdata(dev);
	struct memstick_host *msh = host->msh;

	memstick_resume_host(msh);
	host->system_suspending = false;

	return 0;
}