int rsi_config_wowlan(struct rsi_hw *adapter, struct cfg80211_wowlan *wowlan)
{
	struct rsi_common *common = adapter->priv;
	u16 triggers = 0;
	u16 rx_filter_word = 0;
	struct ieee80211_bss_conf *bss = NULL;

	rsi_dbg(INFO_ZONE, "Config WoWLAN to device\n");

	if (!adapter->vifs[0])
		return -EINVAL;

	bss = &adapter->vifs[0]->bss_conf;

	if (WARN_ON(!wowlan)) {
		rsi_dbg(ERR_ZONE, "WoW triggers not enabled\n");
		return -EINVAL;
	}

	common->wow_flags |= RSI_WOW_ENABLED;
	triggers = rsi_wow_map_triggers(common, wowlan);
	if (!triggers) {
		rsi_dbg(ERR_ZONE, "%s:No valid WoW triggers\n", __func__);
		return -EINVAL;
	}
	if (!bss->assoc) {
		rsi_dbg(ERR_ZONE,
			"Cannot configure WoWLAN (Station not connected)\n");
		common->wow_flags |= RSI_WOW_NO_CONNECTION;
		return 0;
	}
	rsi_dbg(INFO_ZONE, "TRIGGERS %x\n", triggers);
	rsi_send_wowlan_request(common, triggers, 1);

	/**
	 * Increase the beacon_miss threshold & keep-alive timers in
	 * vap_update frame
	 */
	rsi_send_vap_dynamic_update(common);

	rx_filter_word = (ALLOW_DATA_ASSOC_PEER | DISALLOW_BEACONS);
	rsi_send_rx_filter_frame(common, rx_filter_word);

	return 0;
}