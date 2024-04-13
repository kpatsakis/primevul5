static void mcba_usb_xmit_cmd(struct mcba_priv *priv,
			      struct mcba_usb_msg *usb_msg)
{
	struct mcba_usb_ctx *ctx = NULL;
	int err;

	ctx = mcba_usb_get_free_ctx(priv, NULL);
	if (!ctx) {
		netdev_err(priv->netdev,
			   "Lack of free ctx. Sending (%d) cmd aborted",
			   usb_msg->cmd_id);

		return;
	}

	err = mcba_usb_xmit(priv, usb_msg, ctx);
	if (err)
		netdev_err(priv->netdev, "Failed to send cmd (%d)",
			   usb_msg->cmd_id);
}