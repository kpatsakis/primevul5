static int rsi_mac80211_add_interface(struct ieee80211_hw *hw,
				      struct ieee80211_vif *vif)
{
	struct rsi_hw *adapter = hw->priv;
	struct rsi_common *common = adapter->priv;
	struct vif_priv *vif_info = (struct vif_priv *)vif->drv_priv;
	enum opmode intf_mode;
	enum vap_status vap_status;
	int vap_idx = -1, i;

	vif->driver_flags |= IEEE80211_VIF_SUPPORTS_UAPSD;
	mutex_lock(&common->mutex);

	intf_mode = rsi_map_intf_mode(vif->type);
	if (intf_mode == RSI_OPMODE_UNSUPPORTED) {
		rsi_dbg(ERR_ZONE,
			"%s: Interface type %d not supported\n", __func__,
			vif->type);
		mutex_unlock(&common->mutex);
		return -EOPNOTSUPP;
	}
	if ((vif->type == NL80211_IFTYPE_P2P_DEVICE) ||
	    (vif->type == NL80211_IFTYPE_P2P_CLIENT) ||
	    (vif->type == NL80211_IFTYPE_P2P_GO))
		common->p2p_enabled = true;

	/* Get free vap index */
	for (i = 0; i < RSI_MAX_VIFS; i++) {
		if (!adapter->vifs[i]) {
			vap_idx = i;
			break;
		}
	}
	if (vap_idx < 0) {
		rsi_dbg(ERR_ZONE, "Reject: Max VAPs reached\n");
		mutex_unlock(&common->mutex);
		return -EOPNOTSUPP;
	}
	vif_info->vap_id = vap_idx;
	adapter->vifs[vap_idx] = vif;
	adapter->sc_nvifs++;
	vap_status = VAP_ADD;

	if (rsi_set_vap_capabilities(common, intf_mode, vif->addr,
				     vif_info->vap_id, vap_status)) {
		rsi_dbg(ERR_ZONE, "Failed to set VAP capabilities\n");
		mutex_unlock(&common->mutex);
		return -EINVAL;
	}

	if ((vif->type == NL80211_IFTYPE_AP) ||
	    (vif->type == NL80211_IFTYPE_P2P_GO)) {
		rsi_send_rx_filter_frame(common, DISALLOW_BEACONS);
		common->min_rate = RSI_RATE_AUTO;
		for (i = 0; i < common->max_stations; i++)
			common->stations[i].sta = NULL;
	}

	mutex_unlock(&common->mutex);

	return 0;
}