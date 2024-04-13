static void mcba_usb_read_bulk_callback(struct urb *urb)
{
	struct mcba_priv *priv = urb->context;
	struct net_device *netdev;
	int retval;
	int pos = 0;

	netdev = priv->netdev;

	if (!netif_device_present(netdev))
		return;

	switch (urb->status) {
	case 0: /* success */
		break;

	case -ENOENT:
	case -EPIPE:
	case -EPROTO:
	case -ESHUTDOWN:
		return;

	default:
		netdev_info(netdev, "Rx URB aborted (%d)\n", urb->status);

		goto resubmit_urb;
	}

	while (pos < urb->actual_length) {
		struct mcba_usb_msg *msg;

		if (pos + sizeof(struct mcba_usb_msg) > urb->actual_length) {
			netdev_err(priv->netdev, "format error\n");
			break;
		}

		msg = (struct mcba_usb_msg *)(urb->transfer_buffer + pos);
		mcba_usb_process_rx(priv, msg);

		pos += sizeof(struct mcba_usb_msg);
	}

resubmit_urb:

	usb_fill_bulk_urb(urb, priv->udev,
			  usb_rcvbulkpipe(priv->udev, MCBA_USB_EP_OUT),
			  urb->transfer_buffer, MCBA_USB_RX_BUFF_SIZE,
			  mcba_usb_read_bulk_callback, priv);

	retval = usb_submit_urb(urb, GFP_ATOMIC);

	if (retval == -ENODEV)
		netif_device_detach(netdev);
	else if (retval)
		netdev_err(netdev, "failed resubmitting read bulk urb: %d\n",
			   retval);
}