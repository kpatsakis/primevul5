static void mcba_usb_process_ka_usb(struct mcba_priv *priv,
				    struct mcba_usb_msg_ka_usb *msg)
{
	if (unlikely(priv->usb_ka_first_pass)) {
		netdev_info(priv->netdev, "PIC USB version %hhu.%hhu\n",
			    msg->soft_ver_major, msg->soft_ver_minor);

		priv->usb_ka_first_pass = false;
	}

	if (msg->termination_state)
		priv->can.termination = MCBA_TERMINATION_ENABLED;
	else
		priv->can.termination = MCBA_TERMINATION_DISABLED;
}