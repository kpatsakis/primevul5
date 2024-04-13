static void rsi_resume_conn_channel(struct rsi_common *common)
{
	struct rsi_hw *adapter = common->priv;
	struct ieee80211_vif *vif;
	int cnt;

	for (cnt = 0; cnt < RSI_MAX_VIFS; cnt++) {
		vif = adapter->vifs[cnt];
		if (!vif)
			continue;

		if ((vif->type == NL80211_IFTYPE_AP) ||
		    (vif->type == NL80211_IFTYPE_P2P_GO)) {
			rsi_switch_channel(adapter, vif);
			break;
		}
		if (((vif->type == NL80211_IFTYPE_STATION) ||
		     (vif->type == NL80211_IFTYPE_P2P_CLIENT)) &&
		    vif->bss_conf.assoc) {
			rsi_switch_channel(adapter, vif);
			break;
		}
	}
}