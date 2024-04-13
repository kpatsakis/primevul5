static int mcba_net_get_berr_counter(const struct net_device *netdev,
				     struct can_berr_counter *bec)
{
	struct mcba_priv *priv = netdev_priv(netdev);

	bec->txerr = priv->bec.txerr;
	bec->rxerr = priv->bec.rxerr;

	return 0;
}