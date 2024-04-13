static int xennet_xdp_set(struct net_device *dev, struct bpf_prog *prog,
			  struct netlink_ext_ack *extack)
{
	unsigned long max_mtu = XEN_PAGE_SIZE - XDP_PACKET_HEADROOM;
	struct netfront_info *np = netdev_priv(dev);
	struct bpf_prog *old_prog;
	unsigned int i, err;

	if (dev->mtu > max_mtu) {
		netdev_warn(dev, "XDP requires MTU less than %lu\n", max_mtu);
		return -EINVAL;
	}

	if (!np->netback_has_xdp_headroom)
		return 0;

	xenbus_switch_state(np->xbdev, XenbusStateReconfiguring);

	err = talk_to_netback_xdp(np, prog ? NETBACK_XDP_HEADROOM_ENABLE :
				  NETBACK_XDP_HEADROOM_DISABLE);
	if (err)
		return err;

	/* avoid the race with XDP headroom adjustment */
	wait_event(module_wq,
		   xenbus_read_driver_state(np->xbdev->otherend) ==
		   XenbusStateReconfigured);
	np->netfront_xdp_enabled = true;

	old_prog = rtnl_dereference(np->queues[0].xdp_prog);

	if (prog)
		bpf_prog_add(prog, dev->real_num_tx_queues);

	for (i = 0; i < dev->real_num_tx_queues; ++i)
		rcu_assign_pointer(np->queues[i].xdp_prog, prog);

	if (old_prog)
		for (i = 0; i < dev->real_num_tx_queues; ++i)
			bpf_prog_put(old_prog);

	xenbus_switch_state(np->xbdev, XenbusStateConnected);

	return 0;
}