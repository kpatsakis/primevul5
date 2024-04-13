mwifiex_cmd_802_11_ad_hoc_join(struct mwifiex_private *priv,
			       struct host_cmd_ds_command *cmd,
			       struct mwifiex_bssdescriptor *bss_desc)
{
	int rsn_ie_len = 0;
	struct host_cmd_ds_802_11_ad_hoc_join *adhoc_join =
		&cmd->params.adhoc_join;
	struct mwifiex_ie_types_chan_list_param_set *chan_tlv;
	u32 cmd_append_size = 0;
	u16 tmp_cap;
	u32 i, rates_size = 0;
	u16 curr_pkt_filter;
	u8 *pos =
		(u8 *) adhoc_join +
		sizeof(struct host_cmd_ds_802_11_ad_hoc_join);

/* Use G protection */
#define USE_G_PROTECTION        0x02
	if (bss_desc->erp_flags & USE_G_PROTECTION) {
		curr_pkt_filter =
			priv->
			curr_pkt_filter | HostCmd_ACT_MAC_ADHOC_G_PROTECTION_ON;

		if (mwifiex_send_cmd(priv, HostCmd_CMD_MAC_CONTROL,
				     HostCmd_ACT_GEN_SET, 0,
				     &curr_pkt_filter, false)) {
			mwifiex_dbg(priv->adapter, ERROR,
				    "ADHOC_J_CMD: G Protection config failed\n");
			return -1;
		}
	}

	priv->attempted_bss_desc = bss_desc;

	cmd->command = cpu_to_le16(HostCmd_CMD_802_11_AD_HOC_JOIN);

	adhoc_join->bss_descriptor.bss_mode = HostCmd_BSS_MODE_IBSS;

	adhoc_join->bss_descriptor.beacon_period
		= cpu_to_le16(bss_desc->beacon_period);

	memcpy(&adhoc_join->bss_descriptor.bssid,
	       &bss_desc->mac_address, ETH_ALEN);

	memcpy(&adhoc_join->bss_descriptor.ssid,
	       &bss_desc->ssid.ssid, bss_desc->ssid.ssid_len);

	memcpy(&adhoc_join->bss_descriptor.phy_param_set,
	       &bss_desc->phy_param_set,
	       sizeof(union ieee_types_phy_param_set));

	memcpy(&adhoc_join->bss_descriptor.ss_param_set,
	       &bss_desc->ss_param_set, sizeof(union ieee_types_ss_param_set));

	tmp_cap = bss_desc->cap_info_bitmap;

	tmp_cap &= CAPINFO_MASK;

	mwifiex_dbg(priv->adapter, INFO,
		    "info: ADHOC_J_CMD: tmp_cap=%4X CAPINFO_MASK=%4lX\n",
		    tmp_cap, CAPINFO_MASK);

	/* Information on BSSID descriptor passed to FW */
	mwifiex_dbg(priv->adapter, INFO,
		    "info: ADHOC_J_CMD: BSSID=%pM, SSID='%s'\n",
		    adhoc_join->bss_descriptor.bssid,
		    adhoc_join->bss_descriptor.ssid);

	for (i = 0; i < MWIFIEX_SUPPORTED_RATES &&
		    bss_desc->supported_rates[i]; i++)
		;
	rates_size = i;

	/* Copy Data Rates from the Rates recorded in scan response */
	memset(adhoc_join->bss_descriptor.data_rates, 0,
	       sizeof(adhoc_join->bss_descriptor.data_rates));
	memcpy(adhoc_join->bss_descriptor.data_rates,
	       bss_desc->supported_rates, rates_size);

	/* Copy the adhoc join rates into Current BSS state structure */
	priv->curr_bss_params.num_of_rates = rates_size;
	memcpy(&priv->curr_bss_params.data_rates, bss_desc->supported_rates,
	       rates_size);

	/* Copy the channel information */
	priv->curr_bss_params.bss_descriptor.channel = bss_desc->channel;
	priv->curr_bss_params.band = (u8) bss_desc->bss_band;

	if (priv->sec_info.wep_enabled || priv->sec_info.wpa_enabled)
		tmp_cap |= WLAN_CAPABILITY_PRIVACY;

	if (IS_SUPPORT_MULTI_BANDS(priv->adapter)) {
		/* Append a channel TLV */
		chan_tlv = (struct mwifiex_ie_types_chan_list_param_set *) pos;
		chan_tlv->header.type = cpu_to_le16(TLV_TYPE_CHANLIST);
		chan_tlv->header.len =
			cpu_to_le16(sizeof(struct mwifiex_chan_scan_param_set));

		memset(chan_tlv->chan_scan_param, 0x00,
		       sizeof(struct mwifiex_chan_scan_param_set));
		chan_tlv->chan_scan_param[0].chan_number =
			(bss_desc->phy_param_set.ds_param_set.current_chan);
		mwifiex_dbg(priv->adapter, INFO, "info: ADHOC_J_CMD: TLV Chan=%d\n",
			    chan_tlv->chan_scan_param[0].chan_number);

		chan_tlv->chan_scan_param[0].radio_type =
			mwifiex_band_to_radio_type((u8) bss_desc->bss_band);

		mwifiex_dbg(priv->adapter, INFO, "info: ADHOC_J_CMD: TLV Band=%d\n",
			    chan_tlv->chan_scan_param[0].radio_type);
		pos += sizeof(chan_tlv->header) +
				sizeof(struct mwifiex_chan_scan_param_set);
		cmd_append_size += sizeof(chan_tlv->header) +
				sizeof(struct mwifiex_chan_scan_param_set);
	}

	if (priv->sec_info.wpa_enabled)
		rsn_ie_len = mwifiex_append_rsn_ie_wpa_wpa2(priv, &pos);
	if (rsn_ie_len == -1)
		return -1;
	cmd_append_size += rsn_ie_len;

	if (ISSUPP_11NENABLED(priv->adapter->fw_cap_info))
		cmd_append_size += mwifiex_cmd_append_11n_tlv(priv,
			bss_desc, &pos);

	/* Append vendor specific IE TLV */
	cmd_append_size += mwifiex_cmd_append_vsie_tlv(priv,
			MWIFIEX_VSIE_MASK_ADHOC, &pos);

	cmd->size = cpu_to_le16
		((u16) (sizeof(struct host_cmd_ds_802_11_ad_hoc_join)
			+ S_DS_GEN + cmd_append_size));

	adhoc_join->bss_descriptor.cap_info_bitmap = cpu_to_le16(tmp_cap);

	return 0;
}