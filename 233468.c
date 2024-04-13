isdn_net_new(char *name, struct net_device *master)
{
	isdn_net_dev *netdev;

	/* Avoid creating an existing interface */
	if (isdn_net_findif(name)) {
		printk(KERN_WARNING "isdn_net: interface %s already exists\n", name);
		return NULL;
	}
	if (name == NULL)
		return NULL;
	if (!(netdev = kzalloc(sizeof(isdn_net_dev), GFP_KERNEL))) {
		printk(KERN_WARNING "isdn_net: Could not allocate net-device\n");
		return NULL;
	}
	netdev->dev = alloc_netdev(sizeof(isdn_net_local), name, _isdn_setup);
	if (!netdev->dev) {
		printk(KERN_WARNING "isdn_net: Could not allocate network device\n");
		kfree(netdev);
		return NULL;
	}
	netdev->local = netdev->dev->priv;
	netdev->dev->init = isdn_net_init;
	if (master) {
		/* Device shall be a slave */
		struct net_device *p = (((isdn_net_local *) master->priv)->slave);
		struct net_device *q = master;

		netdev->local->master = master;
		/* Put device at end of slave-chain */
		while (p) {
			q = p;
			p = (((isdn_net_local *) p->priv)->slave);
		}
		((isdn_net_local *) q->priv)->slave = netdev->dev;
	} else {
		/* Device shall be a master */
		/*
		 * Watchdog timer (currently) for master only.
		 */
		netdev->dev->tx_timeout = isdn_net_tx_timeout;
		netdev->dev->watchdog_timeo = ISDN_NET_TX_TIMEOUT;
		if (register_netdev(netdev->dev) != 0) {
			printk(KERN_WARNING "isdn_net: Could not register net-device\n");
			free_netdev(netdev->dev);
			kfree(netdev);
			return NULL;
		}
	}
	netdev->queue = netdev->local;
	spin_lock_init(&netdev->queue_lock);

	netdev->local->netdev = netdev;

	INIT_WORK(&netdev->local->tqueue, isdn_net_softint);
	spin_lock_init(&netdev->local->xmit_lock);

	/* Put into to netdev-chain */
	netdev->next = (void *) dev->netdev;
	dev->netdev = netdev;
	return netdev->dev->name;
}