static int rsi_mac80211_cancel_roc(struct ieee80211_hw *hw)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;

	rsi_dbg(INFO_ZONE, "Cancel remain on channel\n");

	mutex_lock(&common->mutex);
	if (!timer_pending(&common->roc_timer)) {
		mutex_unlock(&common->mutex);
		return 0;
	}

	del_timer(&common->roc_timer);

	rsi_resume_conn_channel(common);
	mutex_unlock(&common->mutex);

	return 0;
}