static void rsi_mac80211_stop(struct ieee80211_hw *hw)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;

	rsi_dbg(ERR_ZONE, "===> Interface DOWN <===\n");
	mutex_lock(&common->mutex);
	common->iface_down = true;
	wiphy_rfkill_stop_polling(hw->wiphy);

	/* Block all rx frames */
	rsi_send_rx_filter_frame(common, 0xffff);

	mutex_unlock(&common->mutex);
}