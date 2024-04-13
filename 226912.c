static int mcba_usb_open(struct net_device *netdev)
{
	struct mcba_priv *priv = netdev_priv(netdev);
	int err;

	/* common open */
	err = open_candev(netdev);
	if (err)
		return err;

	priv->can_speed_check = true;
	priv->can.state = CAN_STATE_ERROR_ACTIVE;

	can_led_event(netdev, CAN_LED_EVENT_OPEN);
	netif_start_queue(netdev);

	return 0;
}