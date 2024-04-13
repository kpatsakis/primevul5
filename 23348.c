static bool xennet_can_sg(struct net_device *dev)
{
	return dev->features & NETIF_F_SG;
}