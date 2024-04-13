static int isdn_header_cache(const struct neighbour *neigh, struct hh_cache *hh)
{
	const struct net_device *dev = neigh->dev;
	isdn_net_local *lp = dev->priv;

	if (lp->p_encap == ISDN_NET_ENCAP_ETHER)
		return eth_header_cache(neigh, hh);
	return -1;
}