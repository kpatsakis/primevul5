static void __init sbni_devsetup(struct net_device *dev)
{
	ether_setup( dev );
	dev->open		= &sbni_open;
	dev->stop		= &sbni_close;
	dev->hard_start_xmit	= &sbni_start_xmit;
	dev->get_stats		= &sbni_get_stats;
	dev->set_multicast_list	= &set_multicast_list;
	dev->do_ioctl		= &sbni_ioctl;
}