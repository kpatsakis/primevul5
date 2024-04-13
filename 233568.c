int __init init_module( void )
{
	struct net_device  *dev;
	int err;

	while( num < SBNI_MAX_NUM_CARDS ) {
		dev = alloc_netdev(sizeof(struct net_local), 
				   "sbni%d", sbni_devsetup);
		if( !dev)
			break;

		sprintf( dev->name, "sbni%d", num );

		err = sbni_init(dev);
		if (err) {
			free_netdev(dev);
			break;
		}

		if( register_netdev( dev ) ) {
			release_region( dev->base_addr, SBNI_IO_EXTENT );
			free_netdev( dev );
			break;
		}
	}

	return  *sbni_cards  ?  0  :  -ENODEV;
}