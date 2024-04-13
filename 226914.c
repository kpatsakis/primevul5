static int mcba_set_termination(struct net_device *netdev, u16 term)
{
	struct mcba_priv *priv = netdev_priv(netdev);
	struct mcba_usb_msg_termination usb_msg = {
		.cmd_id = MBCA_CMD_SETUP_TERMINATION_RESISTANCE
	};

	if (term == MCBA_TERMINATION_ENABLED)
		usb_msg.termination = 1;
	else
		usb_msg.termination = 0;

	mcba_usb_xmit_cmd(priv, (struct mcba_usb_msg *)&usb_msg);

	return 0;
}