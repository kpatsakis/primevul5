static int rtsx_usb_ms_runtime_suspend(struct device *dev)
{
	struct rtsx_usb_ms *host = dev_get_drvdata(dev);

	if (host->system_suspending)
		return 0;

	if (host->msh->card || host->power_mode != MEMSTICK_POWER_OFF)
		return -EAGAIN;

	return 0;
}