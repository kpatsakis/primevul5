static int rsi_mac80211_sta_remove(struct ieee80211_hw *hw,
				   struct ieee80211_vif *vif,
				   struct ieee80211_sta *sta)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	struct ieee80211_bss_conf *bss = &vif->bss_conf;
	struct rsi_sta *rsta;

	rsi_dbg(INFO_ZONE, "Station Remove: %pM\n", sta->addr);

	mutex_lock(&common->mutex);

	if ((vif->type == NL80211_IFTYPE_AP) ||
	    (vif->type == NL80211_IFTYPE_P2P_GO)) {
		u8 sta_idx, cnt;

		/* Send peer notify to device */
		rsi_dbg(INFO_ZONE, "Indicate bss status to device\n");
		for (sta_idx = 0; sta_idx < common->max_stations; sta_idx++) {
			rsta = &common->stations[sta_idx];

			if (!rsta->sta)
				continue;
			if (!memcmp(rsta->sta->addr, sta->addr, ETH_ALEN)) {
				rsi_inform_bss_status(common, RSI_OPMODE_AP, 0,
						      sta->addr, sta->wme,
						      sta->aid, sta, sta_idx,
						      0, vif);
				rsta->sta = NULL;
				rsta->sta_id = -1;
				for (cnt = 0; cnt < IEEE80211_NUM_TIDS; cnt++)
					rsta->start_tx_aggr[cnt] = false;
				if (common->num_stations > 0)
					common->num_stations--;
				break;
			}
		}
		if (sta_idx >= common->max_stations)
			rsi_dbg(ERR_ZONE, "%s: No station found\n", __func__);
	}

	if ((vif->type == NL80211_IFTYPE_STATION) ||
	    (vif->type == NL80211_IFTYPE_P2P_CLIENT)) {
		/* Resetting all the fields to default values */
		memcpy((u8 *)bss->bssid, (u8 *)sta->addr, ETH_ALEN);
		bss->qos = sta->wme;
		common->bitrate_mask[NL80211_BAND_2GHZ] = 0;
		common->bitrate_mask[NL80211_BAND_5GHZ] = 0;
		common->min_rate = 0xffff;
		common->vif_info[0].is_ht = false;
		common->vif_info[0].sgi = false;
		common->vif_info[0].seq_start = 0;
		common->secinfo.ptk_cipher = 0;
		common->secinfo.gtk_cipher = 0;
		if (!common->iface_down)
			rsi_send_rx_filter_frame(common, 0);
	}
	mutex_unlock(&common->mutex);
	
	return 0;
}