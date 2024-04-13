static void mcba_usb_xmit_read_fw_ver(struct mcba_priv *priv, u8 pic)
{
	struct mcba_usb_msg_fw_ver usb_msg = {
		.cmd_id = MBCA_CMD_READ_FW_VERSION,
		.pic = pic
	};

	mcba_usb_xmit_cmd(priv, (struct mcba_usb_msg *)&usb_msg);
}