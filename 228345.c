static int test_toggle_sync(struct usbtest_dev *tdev, int ep, struct urb *urb)
{
	int	retval;

	/* clear initial data toggle to DATA0 */
	retval = usb_clear_halt(urb->dev, urb->pipe);
	if (retval < 0) {
		ERROR(tdev, "ep %02x couldn't clear halt, %d\n", ep, retval);
		return retval;
	}

	/* transfer 3 data packets, should be DATA0, DATA1, DATA0 */
	retval = simple_io(tdev, urb, 1, 0, 0, __func__);
	if (retval != 0)
		return -EINVAL;

	/* clear halt resets device side data toggle, host should react to it */
	retval = usb_clear_halt(urb->dev, urb->pipe);
	if (retval < 0) {
		ERROR(tdev, "ep %02x couldn't clear halt, %d\n", ep, retval);
		return retval;
	}

	/* host should use DATA0 again after clear halt */
	retval = simple_io(tdev, urb, 1, 0, 0, __func__);

	return retval;
}