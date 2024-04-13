mwifiex_adhoc_start(struct mwifiex_private *priv,
		    struct cfg80211_ssid *adhoc_ssid)
{
	mwifiex_dbg(priv->adapter, INFO, "info: Adhoc Channel = %d\n",
		    priv->adhoc_channel);
	mwifiex_dbg(priv->adapter, INFO, "info: curr_bss_params.channel = %d\n",
		    priv->curr_bss_params.bss_descriptor.channel);
	mwifiex_dbg(priv->adapter, INFO, "info: curr_bss_params.band = %d\n",
		    priv->curr_bss_params.band);

	if (ISSUPP_11ACENABLED(priv->adapter->fw_cap_info) &&
	    priv->adapter->config_bands & BAND_AAC)
		mwifiex_set_11ac_ba_params(priv);
	else
		mwifiex_set_ba_params(priv);

	return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_AD_HOC_START,
				HostCmd_ACT_GEN_SET, 0, adhoc_ssid, true);
}