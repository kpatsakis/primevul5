static void sg_timeout(struct timer_list *t)
{
	struct sg_timeout *timeout = from_timer(timeout, t, timer);

	usb_sg_cancel(timeout->req);
}