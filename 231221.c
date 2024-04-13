static int mwifiex_deauthenticate_infra(struct mwifiex_private *priv, u8 *mac)
{
	u8 mac_address[ETH_ALEN];
	int ret;

	if (!mac || is_zero_ether_addr(mac))
		memcpy(mac_address,
		       priv->curr_bss_params.bss_descriptor.mac_address,
		       ETH_ALEN);
	else
		memcpy(mac_address, mac, ETH_ALEN);

	ret = mwifiex_send_cmd(priv, HostCmd_CMD_802_11_DEAUTHENTICATE,
			       HostCmd_ACT_GEN_SET, 0, mac_address, true);

	return ret;
}