static int rsi_mac80211_ampdu_action(struct ieee80211_hw *hw,
				     struct ieee80211_vif *vif,
				     struct ieee80211_ampdu_params *params)
{
	int status = -EOPNOTSUPP;
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	struct rsi_sta *rsta = NULL;
	u16 seq_no = 0, seq_start = 0;
	u8 ii = 0;
	struct ieee80211_sta *sta = params->sta;
	u8 sta_id = 0;
	enum ieee80211_ampdu_mlme_action action = params->action;
	u16 tid = params->tid;
	u16 *ssn = &params->ssn;
	u8 buf_size = params->buf_size;

	for (ii = 0; ii < RSI_MAX_VIFS; ii++) {
		if (vif == adapter->vifs[ii])
			break;
	}

	mutex_lock(&common->mutex);

	if (ssn != NULL)
		seq_no = *ssn;

	if ((vif->type == NL80211_IFTYPE_AP) ||
	    (vif->type == NL80211_IFTYPE_P2P_GO)) {
		rsta = rsi_find_sta(common, sta->addr);
		if (!rsta) {
			rsi_dbg(ERR_ZONE, "No station mapped\n");
			status = 0;
			goto unlock;
		}
		sta_id = rsta->sta_id;
	}

	rsi_dbg(INFO_ZONE,
		"%s: AMPDU action tid=%d ssn=0x%x, buf_size=%d sta_id=%d\n",
		__func__, tid, seq_no, buf_size, sta_id);

	switch (action) {
	case IEEE80211_AMPDU_RX_START:
		status = rsi_send_aggregation_params_frame(common,
							   tid,
							   seq_no,
							   buf_size,
							   STA_RX_ADDBA_DONE,
							   sta_id);
		break;

	case IEEE80211_AMPDU_RX_STOP:
		status = rsi_send_aggregation_params_frame(common,
							   tid,
							   0,
							   buf_size,
							   STA_RX_DELBA,
							   sta_id);
		break;

	case IEEE80211_AMPDU_TX_START:
		if ((vif->type == NL80211_IFTYPE_STATION) ||
		    (vif->type == NL80211_IFTYPE_P2P_CLIENT))
			common->vif_info[ii].seq_start = seq_no;
		else if ((vif->type == NL80211_IFTYPE_AP) ||
			 (vif->type == NL80211_IFTYPE_P2P_GO))
			rsta->seq_start[tid] = seq_no;
		ieee80211_start_tx_ba_cb_irqsafe(vif, sta->addr, tid);
		status = 0;
		break;

	case IEEE80211_AMPDU_TX_STOP_CONT:
	case IEEE80211_AMPDU_TX_STOP_FLUSH:
	case IEEE80211_AMPDU_TX_STOP_FLUSH_CONT:
		status = rsi_send_aggregation_params_frame(common,
							   tid,
							   seq_no,
							   buf_size,
							   STA_TX_DELBA,
							   sta_id);
		if (!status)
			ieee80211_stop_tx_ba_cb_irqsafe(vif, sta->addr, tid);
		break;

	case IEEE80211_AMPDU_TX_OPERATIONAL:
		if ((vif->type == NL80211_IFTYPE_STATION) ||
		    (vif->type == NL80211_IFTYPE_P2P_CLIENT))
			seq_start = common->vif_info[ii].seq_start;
		else if ((vif->type == NL80211_IFTYPE_AP) ||
			 (vif->type == NL80211_IFTYPE_P2P_GO))
			seq_start = rsta->seq_start[tid];
		status = rsi_send_aggregation_params_frame(common,
							   tid,
							   seq_start,
							   buf_size,
							   STA_TX_ADDBA_DONE,
							   sta_id);
		break;

	default:
		rsi_dbg(ERR_ZONE, "%s: Uknown AMPDU action\n", __func__);
		break;
	}

unlock:
	mutex_unlock(&common->mutex);
	return status;
}