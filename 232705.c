static int rsi_mac80211_sta_add(struct ieee80211_hw *hw,
				struct ieee80211_vif *vif,
				struct ieee80211_sta *sta)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	bool sta_exist = false;
	struct rsi_sta *rsta;
	int status = 0;

	rsi_dbg(INFO_ZONE, "Station Add: %pM\n", sta->addr);

	mutex_lock(&common->mutex);

	if ((vif->type == NL80211_IFTYPE_AP) ||
	    (vif->type == NL80211_IFTYPE_P2P_GO)) {
		u8 cnt;
		int sta_idx = -1;
		int free_index = -1;

		/* Check if max stations reached */
		if (common->num_stations >= common->max_stations) {
			rsi_dbg(ERR_ZONE, "Reject: Max Stations exists\n");
			status = -EOPNOTSUPP;
			goto unlock;
		}
		for (cnt = 0; cnt < common->max_stations; cnt++) {
			rsta = &common->stations[cnt];

			if (!rsta->sta) {
				if (free_index < 0)
					free_index = cnt;
				continue;
			}
			if (!memcmp(rsta->sta->addr, sta->addr, ETH_ALEN)) {
				rsi_dbg(INFO_ZONE, "Station exists\n");
				sta_idx = cnt;
				sta_exist = true;
				break;
			}
		}
		if (!sta_exist) {
			if (free_index >= 0)
				sta_idx = free_index;
		}
		if (sta_idx < 0) {
			rsi_dbg(ERR_ZONE,
				"%s: Some problem reaching here...\n",
				__func__);
			status = -EINVAL;
			goto unlock;
		}
		rsta = &common->stations[sta_idx];
		rsta->sta = sta;
		rsta->sta_id = sta_idx;
		for (cnt = 0; cnt < IEEE80211_NUM_TIDS; cnt++)
			rsta->start_tx_aggr[cnt] = false;
		for (cnt = 0; cnt < IEEE80211_NUM_TIDS; cnt++)
			rsta->seq_start[cnt] = 0;
		if (!sta_exist) {
			rsi_dbg(INFO_ZONE, "New Station\n");

			/* Send peer notify to device */
			rsi_dbg(INFO_ZONE, "Indicate bss status to device\n");
			rsi_inform_bss_status(common, RSI_OPMODE_AP, 1,
					      sta->addr, sta->wme, sta->aid,
					      sta, sta_idx, 0, vif);

			if (common->key) {
				struct ieee80211_key_conf *key = common->key;

				if ((key->cipher == WLAN_CIPHER_SUITE_WEP104) ||
				    (key->cipher == WLAN_CIPHER_SUITE_WEP40))
					rsi_hal_load_key(adapter->priv,
							 key->key,
							 key->keylen,
							 RSI_PAIRWISE_KEY,
							 key->keyidx,
							 key->cipher,
							 sta_idx,
							 vif);
			}

			common->num_stations++;
		}
	}

	if ((vif->type == NL80211_IFTYPE_STATION) ||
	    (vif->type == NL80211_IFTYPE_P2P_CLIENT)) {
		rsi_set_min_rate(hw, sta, common);
		if (sta->ht_cap.ht_supported) {
			common->vif_info[0].is_ht = true;
			common->bitrate_mask[NL80211_BAND_2GHZ] =
					sta->supp_rates[NL80211_BAND_2GHZ];
			if ((sta->ht_cap.cap & IEEE80211_HT_CAP_SGI_20) ||
			    (sta->ht_cap.cap & IEEE80211_HT_CAP_SGI_40))
				common->vif_info[0].sgi = true;
			ieee80211_start_tx_ba_session(sta, 0, 0);
		}
	}

unlock:
	mutex_unlock(&common->mutex);

	return status;
}