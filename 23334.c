static int xennet_xdp(struct net_device *dev, struct netdev_bpf *xdp)
{
	struct netfront_info *np = netdev_priv(dev);

	if (np->broken)
		return -ENODEV;

	switch (xdp->command) {
	case XDP_SETUP_PROG:
		return xennet_xdp_set(dev, xdp->prog, xdp->extack);
	default:
		return -EINVAL;
	}
}