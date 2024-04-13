int mwifiex_adhoc_join(struct mwifiex_private *priv,
		       struct mwifiex_bssdescriptor *bss_desc)
{
	mwifiex_dbg(priv->adapter, INFO,
		    "info: adhoc join: curr_bss ssid =%s\n",
		    priv->curr_bss_params.bss_descriptor.ssid.ssid);
	mwifiex_dbg(priv->adapter, INFO,
		    "info: adhoc join: curr_bss ssid_len =%u\n",
		    priv->curr_bss_params.bss_descriptor.ssid.ssid_len);
	mwifiex_dbg(priv->adapter, INFO, "info: adhoc join: ssid =%s\n",
		    bss_desc->ssid.ssid);
	mwifiex_dbg(priv->adapter, INFO, "info: adhoc join: ssid_len =%u\n",
		    bss_desc->ssid.ssid_len);

	/* Check if the requested SSID is already joined */
	if (priv->curr_bss_params.bss_descriptor.ssid.ssid_len &&
	    !mwifiex_ssid_cmp(&bss_desc->ssid,
			      &priv->curr_bss_params.bss_descriptor.ssid) &&
	    (priv->curr_bss_params.bss_descriptor.bss_mode ==
							NL80211_IFTYPE_ADHOC)) {
		mwifiex_dbg(priv->adapter, INFO,
			    "info: ADHOC_J_CMD: new ad-hoc SSID\t"
			    "is the same as current; not attempting to re-join\n");
		return -1;
	}

	if (ISSUPP_11ACENABLED(priv->adapter->fw_cap_info) &&
	    !bss_desc->disable_11n && !bss_desc->disable_11ac &&
	    priv->adapter->config_bands & BAND_AAC)
		mwifiex_set_11ac_ba_params(priv);
	else
		mwifiex_set_ba_params(priv);

	mwifiex_dbg(priv->adapter, INFO,
		    "info: curr_bss_params.channel = %d\n",
		    priv->curr_bss_params.bss_descriptor.channel);
	mwifiex_dbg(priv->adapter, INFO,
		    "info: curr_bss_params.band = %c\n",
		    priv->curr_bss_params.band);

	return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_AD_HOC_JOIN,
				HostCmd_ACT_GEN_SET, 0, bss_desc, true);
}