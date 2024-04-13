u16 rsi_get_connected_channel(struct ieee80211_vif *vif)
{
	struct ieee80211_bss_conf *bss;
	struct ieee80211_channel *channel;

	if (!vif)
		return 0;

	bss = &vif->bss_conf;
	channel = bss->chandef.chan;

	if (!channel)
		return 0;

	return channel->hw_value;
}