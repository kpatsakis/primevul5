static int talk_to_netback_xdp(struct netfront_info *np, int xdp)
{
	int err;
	unsigned short headroom;

	headroom = xdp ? XDP_PACKET_HEADROOM : 0;
	err = xenbus_printf(XBT_NIL, np->xbdev->nodename,
			    "xdp-headroom", "%hu",
			    headroom);
	if (err)
		pr_warn("Error writing xdp-headroom\n");

	return err;
}