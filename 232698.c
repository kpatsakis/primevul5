static int rsi_mac80211_suspend(struct ieee80211_hw *hw,
				struct cfg80211_wowlan *wowlan)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;

	rsi_dbg(INFO_ZONE, "%s: mac80211 suspend\n", __func__);
	mutex_lock(&common->mutex);
	if (rsi_config_wowlan(adapter, wowlan)) {
		rsi_dbg(ERR_ZONE, "Failed to configure WoWLAN\n");
		mutex_unlock(&common->mutex);
		return 1;
	}
	mutex_unlock(&common->mutex);

	return 0;
}