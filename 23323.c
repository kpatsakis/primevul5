static int xennet_get_sset_count(struct net_device *dev, int string_set)
{
	switch (string_set) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(xennet_stats);
	default:
		return -EINVAL;
	}
}