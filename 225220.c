static void v4l_print_querycap(const void *arg, bool write_only)
{
	const struct v4l2_capability *p = arg;

	pr_cont("driver=%.*s, card=%.*s, bus=%.*s, version=0x%08x, capabilities=0x%08x, device_caps=0x%08x\n",
		(int)sizeof(p->driver), p->driver,
		(int)sizeof(p->card), p->card,
		(int)sizeof(p->bus_info), p->bus_info,
		p->version, p->capabilities, p->device_caps);
}