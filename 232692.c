static void rsi_mac80211_remove_interface(struct ieee80211_hw *hw,
					  struct ieee80211_vif *vif)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	enum opmode opmode;
	int i;

	rsi_dbg(INFO_ZONE, "Remove Interface Called\n");

	mutex_lock(&common->mutex);

	if (adapter->sc_nvifs <= 0) {
		mutex_unlock(&common->mutex);
		return;
	}

	opmode = rsi_map_intf_mode(vif->type);
	if (opmode == RSI_OPMODE_UNSUPPORTED) {
		rsi_dbg(ERR_ZONE, "Opmode error : %d\n", opmode);
		mutex_unlock(&common->mutex);
		return;
	}
	for (i = 0; i < RSI_MAX_VIFS; i++) {
		if (!adapter->vifs[i])
			continue;
		if (vif == adapter->vifs[i]) {
			rsi_set_vap_capabilities(common, opmode, vif->addr,
						 i, VAP_DELETE);
			adapter->sc_nvifs--;
			adapter->vifs[i] = NULL;
		}
	}
	mutex_unlock(&common->mutex);
}