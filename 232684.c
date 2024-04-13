static int rsi_mac80211_config(struct ieee80211_hw *hw,
			       u32 changed)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	struct ieee80211_conf *conf = &hw->conf;
	int status = -EOPNOTSUPP;

	mutex_lock(&common->mutex);

	if (changed & IEEE80211_CONF_CHANGE_CHANNEL)
		status = rsi_channel_change(hw);

	/* tx power */
	if (changed & IEEE80211_CONF_CHANGE_POWER) {
		rsi_dbg(INFO_ZONE, "%s: Configuring Power\n", __func__);
		status = rsi_config_power(hw);
	}

	/* Power save parameters */
	if (changed & IEEE80211_CONF_CHANGE_PS) {
		struct ieee80211_vif *vif, *sta_vif = NULL;
		unsigned long flags;
		int i, set_ps = 1;

		for (i = 0; i < RSI_MAX_VIFS; i++) {
			vif = adapter->vifs[i];
			if (!vif)
				continue;
			/* Don't go to power save if AP vap exists */
			if ((vif->type == NL80211_IFTYPE_AP) ||
			    (vif->type == NL80211_IFTYPE_P2P_GO)) {
				set_ps = 0;
				break;
			}
			if ((vif->type == NL80211_IFTYPE_STATION ||
			     vif->type == NL80211_IFTYPE_P2P_CLIENT) &&
			    (!sta_vif || vif->bss_conf.assoc))
				sta_vif = vif;
		}
		if (set_ps && sta_vif) {
			spin_lock_irqsave(&adapter->ps_lock, flags);
			if (conf->flags & IEEE80211_CONF_PS)
				rsi_enable_ps(adapter, sta_vif);
			else
				rsi_disable_ps(adapter, sta_vif);
			spin_unlock_irqrestore(&adapter->ps_lock, flags);
		}
	}

	/* RTS threshold */
	if (changed & WIPHY_PARAM_RTS_THRESHOLD) {
		rsi_dbg(INFO_ZONE, "RTS threshold\n");
		if ((common->rts_threshold) <= IEEE80211_MAX_RTS_THRESHOLD) {
			rsi_dbg(INFO_ZONE,
				"%s: Sending vap updates....\n", __func__);
			status = rsi_send_vap_dynamic_update(common);
		}
	}
	mutex_unlock(&common->mutex);

	return status;
}