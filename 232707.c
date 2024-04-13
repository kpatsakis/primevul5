static int rsi_mac80211_start(struct ieee80211_hw *hw)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;

	rsi_dbg(ERR_ZONE, "===> Interface UP <===\n");
	mutex_lock(&common->mutex);
	if (common->hibernate_resume) {
		common->reinit_hw = true;
		adapter->host_intf_ops->reinit_device(adapter);
		wait_for_completion(&adapter->priv->wlan_init_completion);
	}
	common->iface_down = false;
	wiphy_rfkill_start_polling(hw->wiphy);
	rsi_send_rx_filter_frame(common, 0);
	mutex_unlock(&common->mutex);

	return 0;
}