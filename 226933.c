static int mcba_net_set_bittiming(struct net_device *netdev)
{
	struct mcba_priv *priv = netdev_priv(netdev);
	const u16 bitrate_kbps = priv->can.bittiming.bitrate / 1000;

	mcba_usb_xmit_change_bitrate(priv, bitrate_kbps);

	return 0;
}