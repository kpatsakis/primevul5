int mwifiex_deauthenticate(struct mwifiex_private *priv, u8 *mac)
{
	int ret = 0;

	if (!priv->media_connected)
		return 0;

	switch (priv->bss_mode) {
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_P2P_CLIENT:
		ret = mwifiex_deauthenticate_infra(priv, mac);
		if (ret)
			cfg80211_disconnected(priv->netdev, 0, NULL, 0,
					      true, GFP_KERNEL);
		break;
	case NL80211_IFTYPE_ADHOC:
		return mwifiex_send_cmd(priv, HostCmd_CMD_802_11_AD_HOC_STOP,
					HostCmd_ACT_GEN_SET, 0, NULL, true);
	case NL80211_IFTYPE_AP:
		return mwifiex_send_cmd(priv, HostCmd_CMD_UAP_BSS_STOP,
					HostCmd_ACT_GEN_SET, 0, NULL, true);
	default:
		break;
	}

	return ret;
}