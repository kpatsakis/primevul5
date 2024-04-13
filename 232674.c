static int rsi_hal_key_config(struct ieee80211_hw *hw,
			      struct ieee80211_vif *vif,
			      struct ieee80211_key_conf *key,
			      struct ieee80211_sta *sta)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_sta *rsta = NULL;
	int status;
	u8 key_type;
	s16 sta_id = 0;

	if (key->flags & IEEE80211_KEY_FLAG_PAIRWISE)
		key_type = RSI_PAIRWISE_KEY;
	else
		key_type = RSI_GROUP_KEY;

	rsi_dbg(ERR_ZONE, "%s: Cipher 0x%x key_type: %d key_len: %d\n",
		__func__, key->cipher, key_type, key->keylen);

	if ((vif->type == NL80211_IFTYPE_AP) ||
	    (vif->type == NL80211_IFTYPE_P2P_GO)) {
		if (sta) {
			rsta = rsi_find_sta(adapter->priv, sta->addr);
			if (rsta)
				sta_id = rsta->sta_id;
		}
		adapter->priv->key = key;
	} else {
		if ((key->cipher == WLAN_CIPHER_SUITE_WEP104) ||
		    (key->cipher == WLAN_CIPHER_SUITE_WEP40)) {
			status = rsi_hal_load_key(adapter->priv,
						  key->key,
						  key->keylen,
						  RSI_PAIRWISE_KEY,
						  key->keyidx,
						  key->cipher,
						  sta_id,
						  vif);
			if (status)
				return status;
		}
	}

	status = rsi_hal_load_key(adapter->priv,
				  key->key,
				  key->keylen,
				  key_type,
				  key->keyidx,
				  key->cipher,
				  sta_id,
				  vif);
	if (status)
		return status;

	if (vif->type == NL80211_IFTYPE_STATION && key->key &&
	    (key->cipher == WLAN_CIPHER_SUITE_WEP104 ||
	     key->cipher == WLAN_CIPHER_SUITE_WEP40)) {
		if (!rsi_send_block_unblock_frame(adapter->priv, false))
			adapter->priv->hw_data_qs_blocked = false;
	}

	return 0;
}