static int rsi_mac80211_resume(struct ieee80211_hw *hw)
{
	u16 rx_filter_word = 0;
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;

	common->wow_flags = 0;

	rsi_dbg(INFO_ZONE, "%s: mac80211 resume\n", __func__);

	if (common->hibernate_resume)
		return 0;

	mutex_lock(&common->mutex);
	rsi_send_wowlan_request(common, 0, 0);

	rx_filter_word = (ALLOW_DATA_ASSOC_PEER | ALLOW_CTRL_ASSOC_PEER |
			  ALLOW_MGMT_ASSOC_PEER);
	rsi_send_rx_filter_frame(common, rx_filter_word);
	mutex_unlock(&common->mutex);

	return 0;
}