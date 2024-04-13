static void rsi_switch_channel(struct rsi_hw *adapter,
			       struct ieee80211_vif *vif)
{
	struct rsi_common *common = adapter->priv;
	struct ieee80211_channel *channel;

	if (common->iface_down)
		return;
	if (!vif)
		return;

	channel = vif->bss_conf.chandef.chan;

	if (!channel)
		return;

	rsi_band_check(common, channel);
	rsi_set_channel(common, channel);
	rsi_dbg(INFO_ZONE, "Switched to channel - %d\n", channel->hw_value);
}