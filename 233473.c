isdn_net_get_stats(struct net_device *dev)
{
	isdn_net_local *lp = (isdn_net_local *) dev->priv;
	return &lp->stats;
}