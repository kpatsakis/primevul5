static void mcba_usb_write_bulk_callback(struct urb *urb)
{
	struct mcba_usb_ctx *ctx = urb->context;
	struct net_device *netdev;

	WARN_ON(!ctx);

	netdev = ctx->priv->netdev;

	/* free up our allocated buffer */
	usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			  urb->transfer_buffer, urb->transfer_dma);

	if (ctx->can) {
		if (!netif_device_present(netdev))
			return;

		netdev->stats.tx_packets++;
		netdev->stats.tx_bytes += ctx->dlc;

		can_led_event(netdev, CAN_LED_EVENT_TX);
		can_get_echo_skb(netdev, ctx->ndx);
	}

	if (urb->status)
		netdev_info(netdev, "Tx URB aborted (%d)\n", urb->status);

	/* Release the context */
	mcba_usb_free_ctx(ctx);
}