static int rtsx_usb_ms_suspend(struct device *dev)
{
	struct rtsx_usb_ms *host = dev_get_drvdata(dev);
	struct memstick_host *msh = host->msh;

	/* Since we use rtsx_usb's resume callback to runtime resume its
	 * children to implement remote wakeup signaling, this causes
	 * rtsx_usb_ms' runtime resume callback runs after its suspend
	 * callback:
	 * rtsx_usb_ms_suspend()
	 * rtsx_usb_resume()
	 *   -> rtsx_usb_ms_runtime_resume()
	 *     -> memstick_detect_change()
	 *
	 * rtsx_usb_suspend()
	 *
	 * To avoid this, skip runtime resume/suspend if system suspend is
	 * underway.
	 */

	host->system_suspending = true;
	memstick_suspend_host(msh);

	return 0;
}