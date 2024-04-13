static int mcba_usb_probe(struct usb_interface *intf,
			  const struct usb_device_id *id)
{
	struct net_device *netdev;
	struct mcba_priv *priv;
	int err = -ENOMEM;
	struct usb_device *usbdev = interface_to_usbdev(intf);

	netdev = alloc_candev(sizeof(struct mcba_priv), MCBA_MAX_TX_URBS);
	if (!netdev) {
		dev_err(&intf->dev, "Couldn't alloc candev\n");
		return -ENOMEM;
	}

	priv = netdev_priv(netdev);

	priv->udev = usbdev;
	priv->netdev = netdev;
	priv->usb_ka_first_pass = true;
	priv->can_ka_first_pass = true;
	priv->can_speed_check = false;

	init_usb_anchor(&priv->rx_submitted);
	init_usb_anchor(&priv->tx_submitted);

	usb_set_intfdata(intf, priv);

	/* Init CAN device */
	priv->can.state = CAN_STATE_STOPPED;
	priv->can.termination_const = mcba_termination;
	priv->can.termination_const_cnt = ARRAY_SIZE(mcba_termination);
	priv->can.bitrate_const = mcba_bitrate;
	priv->can.bitrate_const_cnt = ARRAY_SIZE(mcba_bitrate);

	priv->can.do_set_termination = mcba_set_termination;
	priv->can.do_set_mode = mcba_net_set_mode;
	priv->can.do_get_berr_counter = mcba_net_get_berr_counter;
	priv->can.do_set_bittiming = mcba_net_set_bittiming;

	netdev->netdev_ops = &mcba_netdev_ops;

	netdev->flags |= IFF_ECHO; /* we support local echo */

	SET_NETDEV_DEV(netdev, &intf->dev);

	err = register_candev(netdev);
	if (err) {
		netdev_err(netdev, "couldn't register CAN device: %d\n", err);

		goto cleanup_free_candev;
	}

	devm_can_led_init(netdev);

	/* Start USB dev only if we have successfully registered CAN device */
	err = mcba_usb_start(priv);
	if (err) {
		if (err == -ENODEV)
			netif_device_detach(priv->netdev);

		netdev_warn(netdev, "couldn't start device: %d\n", err);

		goto cleanup_unregister_candev;
	}

	dev_info(&intf->dev, "Microchip CAN BUS Analyzer connected\n");

	return 0;

cleanup_unregister_candev:
	unregister_candev(priv->netdev);

cleanup_free_candev:
	free_candev(netdev);

	return err;
}