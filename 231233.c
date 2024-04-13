int mwifiex_associate(struct mwifiex_private *priv,
		      struct mwifiex_bssdescriptor *bss_desc)
{
	/* Return error if the adapter is not STA role or table entry
	 * is not marked as infra.
	 */
	if ((GET_BSS_ROLE(priv) != MWIFIEX_BSS_ROLE_STA) ||
	    (bss_desc->bss_mode != NL80211_IFTYPE_STATION))
		return -1;

	if (ISSUPP_11ACENABLED(priv->adapter->fw_cap_info) &&
	    !bss_desc->disable_11n && !bss_desc->disable_11ac &&
	    priv->adapter->config_bands & BAND_AAC)
		mwifiex_set_11ac_ba_params(priv);
	else
		mwifiex_set_ba_params(priv);

	/* Clear any past association response stored for application
	   retrieval */
	priv->assoc_rsp_size = 0;

	return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_ASSOCIATE,
				HostCmd_ACT_GEN_SET, 0, bss_desc, true);
}