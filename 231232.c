int mwifiex_ret_802_11_associate(struct mwifiex_private *priv,
			     struct host_cmd_ds_command *resp)
{
	struct mwifiex_adapter *adapter = priv->adapter;
	int ret = 0;
	struct ieee_types_assoc_rsp *assoc_rsp;
	struct mwifiex_bssdescriptor *bss_desc;
	bool enable_data = true;
	u16 cap_info, status_code, aid;
	const u8 *ie_ptr;
	struct ieee80211_ht_operation *assoc_resp_ht_oper;

	if (!priv->attempted_bss_desc) {
		mwifiex_dbg(priv->adapter, ERROR,
			    "ASSOC_RESP: failed, association terminated by host\n");
		goto done;
	}

	assoc_rsp = (struct ieee_types_assoc_rsp *) &resp->params;

	cap_info = le16_to_cpu(assoc_rsp->cap_info_bitmap);
	status_code = le16_to_cpu(assoc_rsp->status_code);
	aid = le16_to_cpu(assoc_rsp->a_id);

	if ((aid & (BIT(15) | BIT(14))) != (BIT(15) | BIT(14)))
		dev_err(priv->adapter->dev,
			"invalid AID value 0x%x; bits 15:14 not set\n",
			aid);

	aid &= ~(BIT(15) | BIT(14));

	priv->assoc_rsp_size = min(le16_to_cpu(resp->size) - S_DS_GEN,
				   sizeof(priv->assoc_rsp_buf));

	assoc_rsp->a_id = cpu_to_le16(aid);
	memcpy(priv->assoc_rsp_buf, &resp->params, priv->assoc_rsp_size);

	if (status_code) {
		priv->adapter->dbg.num_cmd_assoc_failure++;
		mwifiex_dbg(priv->adapter, ERROR,
			    "ASSOC_RESP: failed,\t"
			    "status code=%d err=%#x a_id=%#x\n",
			    status_code, cap_info,
			    le16_to_cpu(assoc_rsp->a_id));

		mwifiex_dbg(priv->adapter, ERROR, "assoc failure: reason %s\n",
			    assoc_failure_reason_to_str(cap_info));
		if (cap_info == CONNECT_ERR_ASSOC_ERR_TIMEOUT) {
			if (status_code == MWIFIEX_ASSOC_CMD_FAILURE_AUTH) {
				ret = WLAN_STATUS_AUTH_TIMEOUT;
				mwifiex_dbg(priv->adapter, ERROR,
					    "ASSOC_RESP: AUTH timeout\n");
			} else {
				ret = WLAN_STATUS_UNSPECIFIED_FAILURE;
				mwifiex_dbg(priv->adapter, ERROR,
					    "ASSOC_RESP: UNSPECIFIED failure\n");
			}
		} else {
			ret = status_code;
		}

		goto done;
	}

	/* Send a Media Connected event, according to the Spec */
	priv->media_connected = true;

	priv->adapter->ps_state = PS_STATE_AWAKE;
	priv->adapter->pps_uapsd_mode = false;
	priv->adapter->tx_lock_flag = false;

	/* Set the attempted BSSID Index to current */
	bss_desc = priv->attempted_bss_desc;

	mwifiex_dbg(priv->adapter, INFO, "info: ASSOC_RESP: %s\n",
		    bss_desc->ssid.ssid);

	/* Make a copy of current BSSID descriptor */
	memcpy(&priv->curr_bss_params.bss_descriptor,
	       bss_desc, sizeof(struct mwifiex_bssdescriptor));

	/* Update curr_bss_params */
	priv->curr_bss_params.bss_descriptor.channel
		= bss_desc->phy_param_set.ds_param_set.current_chan;

	priv->curr_bss_params.band = (u8) bss_desc->bss_band;

	if (bss_desc->wmm_ie.vend_hdr.element_id == WLAN_EID_VENDOR_SPECIFIC)
		priv->curr_bss_params.wmm_enabled = true;
	else
		priv->curr_bss_params.wmm_enabled = false;

	if ((priv->wmm_required || bss_desc->bcn_ht_cap) &&
	    priv->curr_bss_params.wmm_enabled)
		priv->wmm_enabled = true;
	else
		priv->wmm_enabled = false;

	priv->curr_bss_params.wmm_uapsd_enabled = false;

	if (priv->wmm_enabled)
		priv->curr_bss_params.wmm_uapsd_enabled
			= ((bss_desc->wmm_ie.qos_info_bitmap &
				IEEE80211_WMM_IE_AP_QOSINFO_UAPSD) ? 1 : 0);

	/* Store the bandwidth information from assoc response */
	ie_ptr = cfg80211_find_ie(WLAN_EID_HT_OPERATION, assoc_rsp->ie_buffer,
				  priv->assoc_rsp_size
				  - sizeof(struct ieee_types_assoc_rsp));
	if (ie_ptr) {
		assoc_resp_ht_oper = (struct ieee80211_ht_operation *)(ie_ptr
					+ sizeof(struct ieee_types_header));
		priv->assoc_resp_ht_param = assoc_resp_ht_oper->ht_param;
		priv->ht_param_present = true;
	} else {
		priv->ht_param_present = false;
	}

	mwifiex_dbg(priv->adapter, INFO,
		    "info: ASSOC_RESP: curr_pkt_filter is %#x\n",
		    priv->curr_pkt_filter);
	if (priv->sec_info.wpa_enabled || priv->sec_info.wpa2_enabled)
		priv->wpa_is_gtk_set = false;

	if (priv->wmm_enabled) {
		/* Don't re-enable carrier until we get the WMM_GET_STATUS
		   event */
		enable_data = false;
	} else {
		/* Since WMM is not enabled, setup the queues with the
		   defaults */
		mwifiex_wmm_setup_queue_priorities(priv, NULL);
		mwifiex_wmm_setup_ac_downgrade(priv);
	}

	if (enable_data)
		mwifiex_dbg(priv->adapter, INFO,
			    "info: post association, re-enabling data flow\n");

	/* Reset SNR/NF/RSSI values */
	priv->data_rssi_last = 0;
	priv->data_nf_last = 0;
	priv->data_rssi_avg = 0;
	priv->data_nf_avg = 0;
	priv->bcn_rssi_last = 0;
	priv->bcn_nf_last = 0;
	priv->bcn_rssi_avg = 0;
	priv->bcn_nf_avg = 0;
	priv->rxpd_rate = 0;
	priv->rxpd_htinfo = 0;

	mwifiex_save_curr_bcn(priv);

	priv->adapter->dbg.num_cmd_assoc_success++;

	mwifiex_dbg(priv->adapter, INFO, "info: ASSOC_RESP: associated\n");

	/* Add the ra_list here for infra mode as there will be only 1 ra
	   always */
	mwifiex_ralist_add(priv,
			   priv->curr_bss_params.bss_descriptor.mac_address);

	if (!netif_carrier_ok(priv->netdev))
		netif_carrier_on(priv->netdev);
	mwifiex_wake_up_net_dev_queue(priv->netdev, adapter);

	if (priv->sec_info.wpa_enabled || priv->sec_info.wpa2_enabled)
		priv->scan_block = true;
	else
		priv->port_open = true;

done:
	/* Need to indicate IOCTL complete */
	if (adapter->curr_cmd->wait_q_enabled) {
		if (ret)
			adapter->cmd_wait_q.status = -1;
		else
			adapter->cmd_wait_q.status = 0;
	}

	return ret;
}