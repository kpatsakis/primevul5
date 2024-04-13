void rsi_roc_timeout(struct timer_list *t)
{
	struct rsi_common *common = from_timer(common, t, roc_timer);

	rsi_dbg(INFO_ZONE, "Remain on channel expired\n");

	mutex_lock(&common->mutex);
	ieee80211_remain_on_channel_expired(common->priv->hw);

	if (timer_pending(&common->roc_timer))
		del_timer(&common->roc_timer);

	rsi_resume_conn_channel(common);
	mutex_unlock(&common->mutex);
}