static void mcba_usb_xmit_change_bitrate(struct mcba_priv *priv, u16 bitrate)
{
	struct mcba_usb_msg_change_bitrate usb_msg = {
		.cmd_id = MBCA_CMD_CHANGE_BIT_RATE
	};

	put_unaligned_be16(bitrate, &usb_msg.bitrate);

	mcba_usb_xmit_cmd(priv, (struct mcba_usb_msg *)&usb_msg);
}