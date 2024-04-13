static int perform_sglist(
	struct usbtest_dev	*tdev,
	unsigned		iterations,
	int			pipe,
	struct usb_sg_request	*req,
	struct scatterlist	*sg,
	int			nents
)
{
	struct usb_device	*udev = testdev_to_usbdev(tdev);
	int			retval = 0;
	struct sg_timeout	timeout = {
		.req = req,
	};

	timer_setup_on_stack(&timeout.timer, sg_timeout, 0);

	while (retval == 0 && iterations-- > 0) {
		retval = usb_sg_init(req, udev, pipe,
				(udev->speed == USB_SPEED_HIGH)
					? (INTERRUPT_RATE << 3)
					: INTERRUPT_RATE,
				sg, nents, 0, GFP_KERNEL);

		if (retval)
			break;
		mod_timer(&timeout.timer, jiffies +
				msecs_to_jiffies(SIMPLE_IO_TIMEOUT));
		usb_sg_wait(req);
		if (!del_timer_sync(&timeout.timer))
			retval = -ETIMEDOUT;
		else
			retval = req->status;
		destroy_timer_on_stack(&timeout.timer);

		/* FIXME check resulting data pattern */

		/* FIXME if endpoint halted, clear halt (and log) */
	}

	/* FIXME for unlink or fault handling tests, don't report
	 * failure if retval is as we expected ...
	 */
	if (retval)
		ERROR(tdev, "perform_sglist failed, "
				"iterations left %d, status %d\n",
				iterations, retval);
	return retval;
}