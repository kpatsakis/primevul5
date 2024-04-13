static int rsi_mac80211_roc(struct ieee80211_hw *hw, struct ieee80211_vif *vif,
			    struct ieee80211_channel *chan, int duration,
			    enum ieee80211_roc_type type)
{
	struct rsi_hw *adapter = (struct rsi_hw *)hw->priv;
	struct rsi_common *common = (struct rsi_common *)adapter->priv;
	int status = 0;

	rsi_dbg(INFO_ZONE, "***** Remain on channel *****\n");

	mutex_lock(&common->mutex);
	rsi_dbg(INFO_ZONE, "%s: channel: %d duration: %dms\n",
		__func__, chan->hw_value, duration);

	if (timer_pending(&common->roc_timer)) {
		rsi_dbg(INFO_ZONE, "Stop on-going ROC\n");
		del_timer(&common->roc_timer);
	}
	common->roc_timer.expires = msecs_to_jiffies(duration) + jiffies;
	add_timer(&common->roc_timer);

	/* Configure band */
	if (rsi_band_check(common, chan)) {
		rsi_dbg(ERR_ZONE, "Failed to set band\n");
		status = -EINVAL;
		goto out;
	}

	/* Configure channel */
	if (rsi_set_channel(common, chan)) {
		rsi_dbg(ERR_ZONE, "Failed to set the channel\n");
		status = -EINVAL;
		goto out;
	}

	common->roc_vif = vif;
	ieee80211_ready_on_channel(hw);
	rsi_dbg(INFO_ZONE, "%s: Ready on channel :%d\n",
		__func__, chan->hw_value);

out:
	mutex_unlock(&common->mutex);

	return status;
}