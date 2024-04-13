static void mcba_usb_process_ka_can(struct mcba_priv *priv,
				    struct mcba_usb_msg_ka_can *msg)
{
	if (unlikely(priv->can_ka_first_pass)) {
		netdev_info(priv->netdev, "PIC CAN version %hhu.%hhu\n",
			    msg->soft_ver_major, msg->soft_ver_minor);

		priv->can_ka_first_pass = false;
	}

	if (unlikely(priv->can_speed_check)) {
		const u32 bitrate = convert_can2host_bitrate(msg);

		priv->can_speed_check = false;

		if (bitrate != priv->can.bittiming.bitrate)
			netdev_err(
			    priv->netdev,
			    "Wrong bitrate reported by the device (%u). Expected %u",
			    bitrate, priv->can.bittiming.bitrate);
	}

	priv->bec.txerr = msg->tx_err_cnt;
	priv->bec.rxerr = msg->rx_err_cnt;

	if (msg->tx_bus_off)
		priv->can.state = CAN_STATE_BUS_OFF;

	else if ((priv->bec.txerr > MCBA_CAN_STATE_ERR_PSV_TH) ||
		 (priv->bec.rxerr > MCBA_CAN_STATE_ERR_PSV_TH))
		priv->can.state = CAN_STATE_ERROR_PASSIVE;

	else if ((priv->bec.txerr > MCBA_CAN_STATE_WRN_TH) ||
		 (priv->bec.rxerr > MCBA_CAN_STATE_WRN_TH))
		priv->can.state = CAN_STATE_ERROR_WARNING;
}