static void rsi_mac80211_bss_info_changed(struct ieee80211_hw *hw,
					  struct ieee80211_vif *vif,
					  struct ieee80211_bss_conf *bss_conf,
					  u32 changed)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	struct ieee80211_bss_conf *bss = &vif->bss_conf;
	struct ieee80211_conf *conf = &hw->conf;
	u16 rx_filter_word = 0;

	mutex_lock(&common->mutex);
	if (changed & BSS_CHANGED_ASSOC) {
		rsi_dbg(INFO_ZONE, "%s: Changed Association status: %d\n",
			__func__, bss_conf->assoc);
		if (bss_conf->assoc) {
			/* Send the RX filter frame */
			rx_filter_word = (ALLOW_DATA_ASSOC_PEER |
					  ALLOW_CTRL_ASSOC_PEER |
					  ALLOW_MGMT_ASSOC_PEER);
			rsi_send_rx_filter_frame(common, rx_filter_word);
		}
		rsi_inform_bss_status(common,
				      RSI_OPMODE_STA,
				      bss_conf->assoc,
				      bss_conf->bssid,
				      bss_conf->qos,
				      bss_conf->aid,
				      NULL, 0,
				      bss_conf->assoc_capability, vif);
		adapter->ps_info.dtim_interval_duration = bss->dtim_period;
		adapter->ps_info.listen_interval = conf->listen_interval;

	/* If U-APSD is updated, send ps parameters to firmware */
	if (bss->assoc) {
		if (common->uapsd_bitmap) {
			rsi_dbg(INFO_ZONE, "Configuring UAPSD\n");
			rsi_conf_uapsd(adapter, vif);
		}
	} else {
		common->uapsd_bitmap = 0;
	}
	}

	if (changed & BSS_CHANGED_CQM) {
		common->cqm_info.last_cqm_event_rssi = 0;
		common->cqm_info.rssi_thold = bss_conf->cqm_rssi_thold;
		common->cqm_info.rssi_hyst = bss_conf->cqm_rssi_hyst;
		rsi_dbg(INFO_ZONE, "RSSI throld & hysteresis are: %d %d\n",
			common->cqm_info.rssi_thold,
			common->cqm_info.rssi_hyst);
	}

	if ((changed & BSS_CHANGED_BEACON_ENABLED) &&
	    ((vif->type == NL80211_IFTYPE_AP) ||
	     (vif->type == NL80211_IFTYPE_P2P_GO))) {
		if (bss->enable_beacon) {
			rsi_dbg(INFO_ZONE, "===> BEACON ENABLED <===\n");
			common->beacon_enabled = 1;
		} else {
			rsi_dbg(INFO_ZONE, "===> BEACON DISABLED <===\n");
			common->beacon_enabled = 0;
		}
	}

	mutex_unlock(&common->mutex);
}