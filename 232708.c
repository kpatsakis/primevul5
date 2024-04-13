int rsi_mac80211_attach(struct rsi_common *common)
{
	int status = 0;
	struct ieee80211_hw *hw = NULL;
	struct wiphy *wiphy = NULL;
	struct rsi_hw *adapter = common->priv;
	u8 addr_mask[ETH_ALEN] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x3};

	rsi_dbg(INIT_ZONE, "%s: Performing mac80211 attach\n", __func__);

	hw = ieee80211_alloc_hw(sizeof(struct rsi_hw), &mac80211_ops);
	if (!hw) {
		rsi_dbg(ERR_ZONE, "%s: ieee80211 hw alloc failed\n", __func__);
		return -ENOMEM;
	}

	wiphy = hw->wiphy;

	SET_IEEE80211_DEV(hw, adapter->device);

	hw->priv = adapter;
	adapter->hw = hw;

	ieee80211_hw_set(hw, SIGNAL_DBM);
	ieee80211_hw_set(hw, HAS_RATE_CONTROL);
	ieee80211_hw_set(hw, AMPDU_AGGREGATION);
	ieee80211_hw_set(hw, SUPPORTS_PS);
	ieee80211_hw_set(hw, SUPPORTS_DYNAMIC_PS);

	hw->queues = MAX_HW_QUEUES;
	hw->extra_tx_headroom = RSI_NEEDED_HEADROOM;

	hw->max_rates = 1;
	hw->max_rate_tries = MAX_RETRIES;
	hw->uapsd_queues = RSI_IEEE80211_UAPSD_QUEUES;
	hw->uapsd_max_sp_len = IEEE80211_WMM_IE_STA_QOSINFO_SP_ALL;

	hw->max_tx_aggregation_subframes = RSI_MAX_TX_AGGR_FRMS;
	hw->max_rx_aggregation_subframes = RSI_MAX_RX_AGGR_FRMS;
	hw->rate_control_algorithm = "AARF";

	SET_IEEE80211_PERM_ADDR(hw, common->mac_addr);
	ether_addr_copy(hw->wiphy->addr_mask, addr_mask);

	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
				 BIT(NL80211_IFTYPE_AP) |
				 BIT(NL80211_IFTYPE_P2P_DEVICE) |
				 BIT(NL80211_IFTYPE_P2P_CLIENT) |
				 BIT(NL80211_IFTYPE_P2P_GO);

	wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;
	wiphy->retry_short = RETRY_SHORT;
	wiphy->retry_long  = RETRY_LONG;
	wiphy->frag_threshold = IEEE80211_MAX_FRAG_THRESHOLD;
	wiphy->rts_threshold = IEEE80211_MAX_RTS_THRESHOLD;
	wiphy->flags = 0;

	wiphy->available_antennas_rx = 1;
	wiphy->available_antennas_tx = 1;

	rsi_register_rates_channels(adapter, NL80211_BAND_2GHZ);
	wiphy->bands[NL80211_BAND_2GHZ] =
		&adapter->sbands[NL80211_BAND_2GHZ];
	if (common->num_supp_bands > 1) {
		rsi_register_rates_channels(adapter, NL80211_BAND_5GHZ);
		wiphy->bands[NL80211_BAND_5GHZ] =
			&adapter->sbands[NL80211_BAND_5GHZ];
	}

	/* AP Parameters */
	wiphy->max_ap_assoc_sta = rsi_max_ap_stas[common->oper_mode - 1];
	common->max_stations = wiphy->max_ap_assoc_sta;
	rsi_dbg(ERR_ZONE, "Max Stations Allowed = %d\n", common->max_stations);
	hw->sta_data_size = sizeof(struct rsi_sta);
	wiphy->flags = WIPHY_FLAG_REPORTS_OBSS;
	wiphy->flags |= WIPHY_FLAG_AP_UAPSD;
	wiphy->features |= NL80211_FEATURE_INACTIVITY_TIMER;
	wiphy->reg_notifier = rsi_reg_notify;

#ifdef CONFIG_PM
	wiphy->wowlan = &rsi_wowlan_support;
#endif

	wiphy_ext_feature_set(wiphy, NL80211_EXT_FEATURE_CQM_RSSI_LIST);

	/* Wi-Fi direct parameters */
	wiphy->flags |= WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL;
	wiphy->flags |= WIPHY_FLAG_OFFCHAN_TX;
	wiphy->max_remain_on_channel_duration = 10000;
	hw->max_listen_interval = 10;
	wiphy->iface_combinations = rsi_iface_combinations;
	wiphy->n_iface_combinations = ARRAY_SIZE(rsi_iface_combinations);

	if (common->coex_mode > 1)
		wiphy->flags |= WIPHY_FLAG_PS_ON_BY_DEFAULT;

	status = ieee80211_register_hw(hw);
	if (status)
		return status;

	return rsi_init_dbgfs(adapter);
}