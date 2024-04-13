static void v4l_print_event_subscription(const void *arg, bool write_only)
{
	const struct v4l2_event_subscription *p = arg;

	pr_cont("type=0x%x, id=0x%x, flags=0x%x\n",
			p->type, p->id, p->flags);
}