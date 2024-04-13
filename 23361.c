static struct net_device *xennet_create_dev(struct xenbus_device *dev)
{
	int err;
	struct net_device *netdev;
	struct netfront_info *np;

	netdev = alloc_etherdev_mq(sizeof(struct netfront_info), xennet_max_queues);
	if (!netdev)
		return ERR_PTR(-ENOMEM);

	np                   = netdev_priv(netdev);
	np->xbdev            = dev;

	np->queues = NULL;

	err = -ENOMEM;
	np->rx_stats = netdev_alloc_pcpu_stats(struct netfront_stats);
	if (np->rx_stats == NULL)
		goto exit;
	np->tx_stats = netdev_alloc_pcpu_stats(struct netfront_stats);
	if (np->tx_stats == NULL)
		goto exit;

	netdev->netdev_ops	= &xennet_netdev_ops;

	netdev->features        = NETIF_F_IP_CSUM | NETIF_F_RXCSUM |
				  NETIF_F_GSO_ROBUST;
	netdev->hw_features	= NETIF_F_SG |
				  NETIF_F_IPV6_CSUM |
				  NETIF_F_TSO | NETIF_F_TSO6;

	/*
         * Assume that all hw features are available for now. This set
         * will be adjusted by the call to netdev_update_features() in
         * xennet_connect() which is the earliest point where we can
         * negotiate with the backend regarding supported features.
         */
	netdev->features |= netdev->hw_features;

	netdev->ethtool_ops = &xennet_ethtool_ops;
	netdev->min_mtu = ETH_MIN_MTU;
	netdev->max_mtu = XEN_NETIF_MAX_TX_SIZE;
	SET_NETDEV_DEV(netdev, &dev->dev);

	np->netdev = netdev;
	np->netfront_xdp_enabled = false;

	netif_carrier_off(netdev);

	do {
		xenbus_switch_state(dev, XenbusStateInitialising);
		err = wait_event_timeout(module_wq,
				 xenbus_read_driver_state(dev->otherend) !=
				 XenbusStateClosed &&
				 xenbus_read_driver_state(dev->otherend) !=
				 XenbusStateUnknown, XENNET_TIMEOUT);
	} while (!err);

	return netdev;

 exit:
	xennet_free_netdev(netdev);
	return ERR_PTR(err);
}