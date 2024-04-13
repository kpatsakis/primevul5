cleanup_module( void )
{
	struct net_device  *dev;
	int  num;

	for( num = 0;  num < SBNI_MAX_NUM_CARDS;  ++num )
		if( (dev = sbni_cards[ num ]) != NULL ) {
			unregister_netdev( dev );
			release_region( dev->base_addr, SBNI_IO_EXTENT );
			free_netdev( dev );
		}
}