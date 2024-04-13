int mwifiex_cmd_802_11_associate(struct mwifiex_private *priv,
				 struct host_cmd_ds_command *cmd,
				 struct mwifiex_bssdescriptor *bss_desc)
{
	struct host_cmd_ds_802_11_associate *assoc = &cmd->params.associate;
	struct mwifiex_ie_types_ssid_param_set *ssid_tlv;
	struct mwifiex_ie_types_phy_param_set *phy_tlv;
	struct mwifiex_ie_types_ss_param_set *ss_tlv;
	struct mwifiex_ie_types_rates_param_set *rates_tlv;
	struct mwifiex_ie_types_auth_type *auth_tlv;
	struct mwifiex_ie_types_chan_list_param_set *chan_tlv;
	u8 rates[MWIFIEX_SUPPORTED_RATES];
	u32 rates_size;
	u16 tmp_cap;
	u8 *pos;
	int rsn_ie_len = 0;

	pos = (u8 *) assoc;

	cmd->command = cpu_to_le16(HostCmd_CMD_802_11_ASSOCIATE);

	/* Save so we know which BSS Desc to use in the response handler */
	priv->attempted_bss_desc = bss_desc;

	memcpy(assoc->peer_sta_addr,
	       bss_desc->mac_address, sizeof(assoc->peer_sta_addr));
	pos += sizeof(assoc->peer_sta_addr);

	/* Set the listen interval */
	assoc->listen_interval = cpu_to_le16(priv->listen_interval);
	/* Set the beacon period */
	assoc->beacon_period = cpu_to_le16(bss_desc->beacon_period);

	pos += sizeof(assoc->cap_info_bitmap);
	pos += sizeof(assoc->listen_interval);
	pos += sizeof(assoc->beacon_period);
	pos += sizeof(assoc->dtim_period);

	ssid_tlv = (struct mwifiex_ie_types_ssid_param_set *) pos;
	ssid_tlv->header.type = cpu_to_le16(WLAN_EID_SSID);
	ssid_tlv->header.len = cpu_to_le16((u16) bss_desc->ssid.ssid_len);
	memcpy(ssid_tlv->ssid, bss_desc->ssid.ssid,
	       le16_to_cpu(ssid_tlv->header.len));
	pos += sizeof(ssid_tlv->header) + le16_to_cpu(ssid_tlv->header.len);

	phy_tlv = (struct mwifiex_ie_types_phy_param_set *) pos;
	phy_tlv->header.type = cpu_to_le16(WLAN_EID_DS_PARAMS);
	phy_tlv->header.len = cpu_to_le16(sizeof(phy_tlv->fh_ds.ds_param_set));
	memcpy(&phy_tlv->fh_ds.ds_param_set,
	       &bss_desc->phy_param_set.ds_param_set.current_chan,
	       sizeof(phy_tlv->fh_ds.ds_param_set));
	pos += sizeof(phy_tlv->header) + le16_to_cpu(phy_tlv->header.len);

	ss_tlv = (struct mwifiex_ie_types_ss_param_set *) pos;
	ss_tlv->header.type = cpu_to_le16(WLAN_EID_CF_PARAMS);
	ss_tlv->header.len = cpu_to_le16(sizeof(ss_tlv->cf_ibss.cf_param_set));
	pos += sizeof(ss_tlv->header) + le16_to_cpu(ss_tlv->header.len);

	/* Get the common rates supported between the driver and the BSS Desc */
	if (mwifiex_setup_rates_from_bssdesc
	    (priv, bss_desc, rates, &rates_size))
		return -1;

	/* Save the data rates into Current BSS state structure */
	priv->curr_bss_params.num_of_rates = rates_size;
	memcpy(&priv->curr_bss_params.data_rates, rates, rates_size);

	/* Setup the Rates TLV in the association command */
	rates_tlv = (struct mwifiex_ie_types_rates_param_set *) pos;
	rates_tlv->header.type = cpu_to_le16(WLAN_EID_SUPP_RATES);
	rates_tlv->header.len = cpu_to_le16((u16) rates_size);
	memcpy(rates_tlv->rates, rates, rates_size);
	pos += sizeof(rates_tlv->header) + rates_size;
	mwifiex_dbg(priv->adapter, INFO, "info: ASSOC_CMD: rates size = %d\n",
		    rates_size);

	/* Add the Authentication type to be used for Auth frames */
	auth_tlv = (struct mwifiex_ie_types_auth_type *) pos;
	auth_tlv->header.type = cpu_to_le16(TLV_TYPE_AUTH_TYPE);
	auth_tlv->header.len = cpu_to_le16(sizeof(auth_tlv->auth_type));
	if (priv->sec_info.wep_enabled)
		auth_tlv->auth_type = cpu_to_le16(
				(u16) priv->sec_info.authentication_mode);
	else
		auth_tlv->auth_type = cpu_to_le16(NL80211_AUTHTYPE_OPEN_SYSTEM);

	pos += sizeof(auth_tlv->header) + le16_to_cpu(auth_tlv->header.len);

	if (IS_SUPPORT_MULTI_BANDS(priv->adapter) &&
	    !(ISSUPP_11NENABLED(priv->adapter->fw_cap_info) &&
	    (!bss_desc->disable_11n) &&
	    (priv->adapter->config_bands & BAND_GN ||
	     priv->adapter->config_bands & BAND_AN) &&
	    (bss_desc->bcn_ht_cap)
	    )
		) {
		/* Append a channel TLV for the channel the attempted AP was
		   found on */
		chan_tlv = (struct mwifiex_ie_types_chan_list_param_set *) pos;
		chan_tlv->header.type = cpu_to_le16(TLV_TYPE_CHANLIST);
		chan_tlv->header.len =
			cpu_to_le16(sizeof(struct mwifiex_chan_scan_param_set));

		memset(chan_tlv->chan_scan_param, 0x00,
		       sizeof(struct mwifiex_chan_scan_param_set));
		chan_tlv->chan_scan_param[0].chan_number =
			(bss_desc->phy_param_set.ds_param_set.current_chan);
		mwifiex_dbg(priv->adapter, INFO, "info: Assoc: TLV Chan = %d\n",
			    chan_tlv->chan_scan_param[0].chan_number);

		chan_tlv->chan_scan_param[0].radio_type =
			mwifiex_band_to_radio_type((u8) bss_desc->bss_band);

		mwifiex_dbg(priv->adapter, INFO, "info: Assoc: TLV Band = %d\n",
			    chan_tlv->chan_scan_param[0].radio_type);
		pos += sizeof(chan_tlv->header) +
			sizeof(struct mwifiex_chan_scan_param_set);
	}

	if (!priv->wps.session_enable) {
		if (priv->sec_info.wpa_enabled || priv->sec_info.wpa2_enabled)
			rsn_ie_len = mwifiex_append_rsn_ie_wpa_wpa2(priv, &pos);

		if (rsn_ie_len == -1)
			return -1;
	}

	if (ISSUPP_11NENABLED(priv->adapter->fw_cap_info) &&
	    (!bss_desc->disable_11n) &&
	    (priv->adapter->config_bands & BAND_GN ||
	     priv->adapter->config_bands & BAND_AN))
		mwifiex_cmd_append_11n_tlv(priv, bss_desc, &pos);

	if (ISSUPP_11ACENABLED(priv->adapter->fw_cap_info) &&
	    !bss_desc->disable_11n && !bss_desc->disable_11ac &&
	    priv->adapter->config_bands & BAND_AAC)
		mwifiex_cmd_append_11ac_tlv(priv, bss_desc, &pos);

	/* Append vendor specific IE TLV */
	mwifiex_cmd_append_vsie_tlv(priv, MWIFIEX_VSIE_MASK_ASSOC, &pos);

	mwifiex_wmm_process_association_req(priv, &pos, &bss_desc->wmm_ie,
					    bss_desc->bcn_ht_cap);
	if (priv->sec_info.wapi_enabled && priv->wapi_ie_len)
		mwifiex_cmd_append_wapi_ie(priv, &pos);

	if (priv->wps.session_enable && priv->wps_ie_len)
		mwifiex_cmd_append_wps_ie(priv, &pos);

	mwifiex_cmd_append_generic_ie(priv, &pos);

	mwifiex_cmd_append_tsf_tlv(priv, &pos, bss_desc);

	mwifiex_11h_process_join(priv, &pos, bss_desc);

	cmd->size = cpu_to_le16((u16) (pos - (u8 *) assoc) + S_DS_GEN);

	/* Set the Capability info at last */
	tmp_cap = bss_desc->cap_info_bitmap;

	if (priv->adapter->config_bands == BAND_B)
		tmp_cap &= ~WLAN_CAPABILITY_SHORT_SLOT_TIME;

	tmp_cap &= CAPINFO_MASK;
	mwifiex_dbg(priv->adapter, INFO,
		    "info: ASSOC_CMD: tmp_cap=%4X CAPINFO_MASK=%4lX\n",
		    tmp_cap, CAPINFO_MASK);
	assoc->cap_info_bitmap = cpu_to_le16(tmp_cap);

	return 0;
}