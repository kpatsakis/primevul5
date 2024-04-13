static void xennet_get_stats64(struct net_device *dev,
			       struct rtnl_link_stats64 *tot)
{
	struct netfront_info *np = netdev_priv(dev);
	int cpu;

	for_each_possible_cpu(cpu) {
		struct netfront_stats *rx_stats = per_cpu_ptr(np->rx_stats, cpu);
		struct netfront_stats *tx_stats = per_cpu_ptr(np->tx_stats, cpu);
		u64 rx_packets, rx_bytes, tx_packets, tx_bytes;
		unsigned int start;

		do {
			start = u64_stats_fetch_begin_irq(&tx_stats->syncp);
			tx_packets = tx_stats->packets;
			tx_bytes = tx_stats->bytes;
		} while (u64_stats_fetch_retry_irq(&tx_stats->syncp, start));

		do {
			start = u64_stats_fetch_begin_irq(&rx_stats->syncp);
			rx_packets = rx_stats->packets;
			rx_bytes = rx_stats->bytes;
		} while (u64_stats_fetch_retry_irq(&rx_stats->syncp, start));

		tot->rx_packets += rx_packets;
		tot->tx_packets += tx_packets;
		tot->rx_bytes   += rx_bytes;
		tot->tx_bytes   += tx_bytes;
	}

	tot->rx_errors  = dev->stats.rx_errors;
	tot->tx_dropped = dev->stats.tx_dropped;
}