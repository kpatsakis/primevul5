static int mcba_usb_close(struct net_device *netdev)
{
	struct mcba_priv *priv = netdev_priv(netdev);

	priv->can.state = CAN_STATE_STOPPED;

	netif_stop_queue(netdev);

	/* Stop polling */
	mcba_urb_unlink(priv);

	close_candev(netdev);
	can_led_event(netdev, CAN_LED_EVENT_STOP);

	return 0;
}