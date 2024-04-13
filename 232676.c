static int rsi_channel_change(struct ieee80211_hw *hw)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	int status = -EOPNOTSUPP;
	struct ieee80211_channel *curchan = hw->conf.chandef.chan;
	u16 channel = curchan->hw_value;
	struct ieee80211_vif *vif;
	struct ieee80211_bss_conf *bss;
	bool assoc = false;
	int i;

	rsi_dbg(INFO_ZONE,
		"%s: Set channel: %d MHz type: %d channel_no %d\n",
		__func__, curchan->center_freq,
		curchan->flags, channel);

	for (i = 0; i < RSI_MAX_VIFS; i++) {
		vif = adapter->vifs[i];
		if (!vif)
			continue;
		if (vif->type == NL80211_IFTYPE_STATION) {
			bss = &vif->bss_conf;
			if (bss->assoc) {
				assoc = true;
				break;
			}
		}
	}
	if (assoc) {
		if (!common->hw_data_qs_blocked &&
		    (rsi_get_connected_channel(vif) != channel)) {
			rsi_dbg(INFO_ZONE, "blk data q %d\n", channel);
			if (!rsi_send_block_unblock_frame(common, true))
				common->hw_data_qs_blocked = true;
		}
	}

	status = rsi_band_check(common, curchan);
	if (!status)
		status = rsi_set_channel(adapter->priv, curchan);

	if (assoc) {
		if (common->hw_data_qs_blocked &&
		    (rsi_get_connected_channel(vif) == channel)) {
			rsi_dbg(INFO_ZONE, "unblk data q %d\n", channel);
			if (!rsi_send_block_unblock_frame(common, false))
				common->hw_data_qs_blocked = false;
		}
	}

	return status;
}