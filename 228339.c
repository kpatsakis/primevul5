static int unlink_simple(struct usbtest_dev *dev, int pipe, int len)
{
	int			retval = 0;

	/* test sync and async paths */
	retval = unlink1(dev, pipe, len, 1);
	if (!retval)
		retval = unlink1(dev, pipe, len, 0);
	return retval;
}