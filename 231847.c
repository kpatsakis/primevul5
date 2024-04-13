static int x25_device_event(struct notifier_block *this, unsigned long event,
			    void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct x25_neigh *nb;

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	if (dev->type == ARPHRD_X25) {
		switch (event) {
		case NETDEV_REGISTER:
		case NETDEV_POST_TYPE_CHANGE:
			x25_link_device_up(dev);
			break;
		case NETDEV_DOWN:
			nb = x25_get_neigh(dev);
			if (nb) {
				x25_link_terminated(nb);
				x25_neigh_put(nb);
			}
			x25_route_device_down(dev);
			break;
		case NETDEV_PRE_TYPE_CHANGE:
		case NETDEV_UNREGISTER:
			x25_link_device_down(dev);
			break;
		case NETDEV_CHANGE:
			if (!netif_carrier_ok(dev)) {
				nb = x25_get_neigh(dev);
				if (nb) {
					x25_link_terminated(nb);
					x25_neigh_put(nb);
				}
			}
			break;
		}
	}

	return NOTIFY_DONE;
}