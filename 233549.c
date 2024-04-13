int __init sbni_probe(int unit)
{
	struct net_device *dev;
	static unsigned  version_printed __initdata = 0;
	int err;

	dev = alloc_netdev(sizeof(struct net_local), "sbni", sbni_devsetup);
	if (!dev)
		return -ENOMEM;

	sprintf(dev->name, "sbni%d", unit);
	netdev_boot_setup_check(dev);

	err = sbni_init(dev);
	if (err) {
		free_netdev(dev);
		return err;
	}

	err = register_netdev(dev);
	if (err) {
		release_region( dev->base_addr, SBNI_IO_EXTENT );
		free_netdev(dev);
		return err;
	}
	if( version_printed++ == 0 )
		printk( KERN_INFO "%s", version );
	return 0;
}