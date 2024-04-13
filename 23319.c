static netdev_features_t xennet_fix_features(struct net_device *dev,
	netdev_features_t features)
{
	struct netfront_info *np = netdev_priv(dev);

	if (features & NETIF_F_SG &&
	    !xenbus_read_unsigned(np->xbdev->otherend, "feature-sg", 0))
		features &= ~NETIF_F_SG;

	if (features & NETIF_F_IPV6_CSUM &&
	    !xenbus_read_unsigned(np->xbdev->otherend,
				  "feature-ipv6-csum-offload", 0))
		features &= ~NETIF_F_IPV6_CSUM;

	if (features & NETIF_F_TSO &&
	    !xenbus_read_unsigned(np->xbdev->otherend, "feature-gso-tcpv4", 0))
		features &= ~NETIF_F_TSO;

	if (features & NETIF_F_TSO6 &&
	    !xenbus_read_unsigned(np->xbdev->otherend, "feature-gso-tcpv6", 0))
		features &= ~NETIF_F_TSO6;

	return features;
}