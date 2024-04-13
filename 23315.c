static void xennet_free_netdev(struct net_device *netdev)
{
	struct netfront_info *np = netdev_priv(netdev);

	free_percpu(np->rx_stats);
	free_percpu(np->tx_stats);
	free_netdev(netdev);
}