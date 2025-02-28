isdn_net_reset(struct net_device *dev)
{
#ifdef CONFIG_ISDN_X25
	struct concap_device_ops * dops =
		( (isdn_net_local *) dev->priv ) -> dops;
	struct concap_proto * cprot =
		( (isdn_net_local *) dev->priv ) -> netdev -> cprot;
#endif
#ifdef CONFIG_ISDN_X25
	if( cprot && cprot -> pops && dops )
		cprot -> pops -> restart ( cprot, dev, dops );
#endif
}