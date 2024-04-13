brcmf_cfg80211_start_ap(struct wiphy *wiphy, struct net_device *ndev,
			struct cfg80211_ap_settings *settings)
{
	s32 ie_offset;
	struct brcmf_cfg80211_info *cfg = wiphy_to_cfg(wiphy);
	struct brcmf_if *ifp = netdev_priv(ndev);
	const struct brcmf_tlv *ssid_ie;
	const struct brcmf_tlv *country_ie;
	struct brcmf_ssid_le ssid_le;
	s32 err = -EPERM;
	const struct brcmf_tlv *rsn_ie;
	const struct brcmf_vs_tlv *wpa_ie;
	struct brcmf_join_params join_params;
	enum nl80211_iftype dev_role;
	struct brcmf_fil_bss_enable_le bss_enable;
	u16 chanspec = chandef_to_chanspec(&cfg->d11inf, &settings->chandef);
	bool mbss;
	int is_11d;

	brcmf_dbg(TRACE, "ctrlchn=%d, center=%d, bw=%d, beacon_interval=%d, dtim_period=%d,\n",
		  settings->chandef.chan->hw_value,
		  settings->chandef.center_freq1, settings->chandef.width,
		  settings->beacon_interval, settings->dtim_period);
	brcmf_dbg(TRACE, "ssid=%s(%zu), auth_type=%d, inactivity_timeout=%d\n",
		  settings->ssid, settings->ssid_len, settings->auth_type,
		  settings->inactivity_timeout);
	dev_role = ifp->vif->wdev.iftype;
	mbss = ifp->vif->mbss;

	/* store current 11d setting */
	brcmf_fil_cmd_int_get(ifp, BRCMF_C_GET_REGULATORY, &ifp->vif->is_11d);
	country_ie = brcmf_parse_tlvs((u8 *)settings->beacon.tail,
				      settings->beacon.tail_len,
				      WLAN_EID_COUNTRY);
	is_11d = country_ie ? 1 : 0;

	memset(&ssid_le, 0, sizeof(ssid_le));
	if (settings->ssid == NULL || settings->ssid_len == 0) {
		ie_offset = DOT11_MGMT_HDR_LEN + DOT11_BCN_PRB_FIXED_LEN;
		ssid_ie = brcmf_parse_tlvs(
 				(u8 *)&settings->beacon.head[ie_offset],
 				settings->beacon.head_len - ie_offset,
 				WLAN_EID_SSID);
		if (!ssid_ie)
 			return -EINVAL;
 
 		memcpy(ssid_le.SSID, ssid_ie->data, ssid_ie->len);
		ssid_le.SSID_len = cpu_to_le32(ssid_ie->len);
		brcmf_dbg(TRACE, "SSID is (%s) in Head\n", ssid_le.SSID);
	} else {
		memcpy(ssid_le.SSID, settings->ssid, settings->ssid_len);
		ssid_le.SSID_len = cpu_to_le32((u32)settings->ssid_len);
	}

	if (!mbss) {
		brcmf_set_mpc(ifp, 0);
		brcmf_configure_arp_nd_offload(ifp, false);
	}

	/* find the RSN_IE */
	rsn_ie = brcmf_parse_tlvs((u8 *)settings->beacon.tail,
				  settings->beacon.tail_len, WLAN_EID_RSN);

	/* find the WPA_IE */
	wpa_ie = brcmf_find_wpaie((u8 *)settings->beacon.tail,
				  settings->beacon.tail_len);

	if ((wpa_ie != NULL || rsn_ie != NULL)) {
		brcmf_dbg(TRACE, "WPA(2) IE is found\n");
		if (wpa_ie != NULL) {
			/* WPA IE */
			err = brcmf_configure_wpaie(ifp, wpa_ie, false);
			if (err < 0)
				goto exit;
		} else {
			struct brcmf_vs_tlv *tmp_ie;

			tmp_ie = (struct brcmf_vs_tlv *)rsn_ie;

			/* RSN IE */
			err = brcmf_configure_wpaie(ifp, tmp_ie, true);
			if (err < 0)
				goto exit;
		}
	} else {
		brcmf_dbg(TRACE, "No WPA(2) IEs found\n");
		brcmf_configure_opensecurity(ifp);
	}

	brcmf_config_ap_mgmt_ie(ifp->vif, &settings->beacon);

	/* Parameters shared by all radio interfaces */
	if (!mbss) {
		if (is_11d != ifp->vif->is_11d) {
			err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_REGULATORY,
						    is_11d);
			if (err < 0) {
				brcmf_err("Regulatory Set Error, %d\n", err);
				goto exit;
			}
		}
		if (settings->beacon_interval) {
			err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_BCNPRD,
						    settings->beacon_interval);
			if (err < 0) {
				brcmf_err("Beacon Interval Set Error, %d\n",
					  err);
				goto exit;
			}
		}
		if (settings->dtim_period) {
			err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_DTIMPRD,
						    settings->dtim_period);
			if (err < 0) {
				brcmf_err("DTIM Interval Set Error, %d\n", err);
				goto exit;
			}
		}

		if ((dev_role == NL80211_IFTYPE_AP) &&
		    ((ifp->ifidx == 0) ||
		     !brcmf_feat_is_enabled(ifp, BRCMF_FEAT_RSDB))) {
			err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_DOWN, 1);
			if (err < 0) {
				brcmf_err("BRCMF_C_DOWN error %d\n", err);
				goto exit;
			}
			brcmf_fil_iovar_int_set(ifp, "apsta", 0);
		}

		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_INFRA, 1);
		if (err < 0) {
			brcmf_err("SET INFRA error %d\n", err);
			goto exit;
		}
	} else if (WARN_ON(is_11d != ifp->vif->is_11d)) {
		/* Multiple-BSS should use same 11d configuration */
		err = -EINVAL;
		goto exit;
	}

	/* Interface specific setup */
	if (dev_role == NL80211_IFTYPE_AP) {
		if ((brcmf_feat_is_enabled(ifp, BRCMF_FEAT_MBSS)) && (!mbss))
			brcmf_fil_iovar_int_set(ifp, "mbss", 1);

		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_SET_AP, 1);
		if (err < 0) {
			brcmf_err("setting AP mode failed %d\n", err);
			goto exit;
		}
		if (!mbss) {
			/* Firmware 10.x requires setting channel after enabling
			 * AP and before bringing interface up.
			 */
			err = brcmf_fil_iovar_int_set(ifp, "chanspec", chanspec);
			if (err < 0) {
				brcmf_err("Set Channel failed: chspec=%d, %d\n",
					  chanspec, err);
				goto exit;
			}
		}
		err = brcmf_fil_cmd_int_set(ifp, BRCMF_C_UP, 1);
		if (err < 0) {
			brcmf_err("BRCMF_C_UP error (%d)\n", err);
			goto exit;
		}
		/* On DOWN the firmware removes the WEP keys, reconfigure
		 * them if they were set.
		 */
		brcmf_cfg80211_reconfigure_wep(ifp);

		memset(&join_params, 0, sizeof(join_params));
		/* join parameters starts with ssid */
		memcpy(&join_params.ssid_le, &ssid_le, sizeof(ssid_le));
		/* create softap */
		err = brcmf_fil_cmd_data_set(ifp, BRCMF_C_SET_SSID,
					     &join_params, sizeof(join_params));
		if (err < 0) {
			brcmf_err("SET SSID error (%d)\n", err);
			goto exit;
		}

		if (settings->hidden_ssid) {
			err = brcmf_fil_iovar_int_set(ifp, "closednet", 1);
			if (err) {
				brcmf_err("closednet error (%d)\n", err);
				goto exit;
			}
		}

		brcmf_dbg(TRACE, "AP mode configuration complete\n");
	} else if (dev_role == NL80211_IFTYPE_P2P_GO) {
		err = brcmf_fil_iovar_int_set(ifp, "chanspec", chanspec);
		if (err < 0) {
			brcmf_err("Set Channel failed: chspec=%d, %d\n",
				  chanspec, err);
			goto exit;
		}
		err = brcmf_fil_bsscfg_data_set(ifp, "ssid", &ssid_le,
						sizeof(ssid_le));
		if (err < 0) {
			brcmf_err("setting ssid failed %d\n", err);
			goto exit;
		}
		bss_enable.bsscfgidx = cpu_to_le32(ifp->bsscfgidx);
		bss_enable.enable = cpu_to_le32(1);
		err = brcmf_fil_iovar_data_set(ifp, "bss", &bss_enable,
					       sizeof(bss_enable));
		if (err < 0) {
			brcmf_err("bss_enable config failed %d\n", err);
			goto exit;
		}

		brcmf_dbg(TRACE, "GO mode configuration complete\n");
	} else {
		WARN_ON(1);
	}

	set_bit(BRCMF_VIF_STATUS_AP_CREATED, &ifp->vif->sme_state);
	brcmf_net_setcarrier(ifp, true);

exit:
	if ((err) && (!mbss)) {
		brcmf_set_mpc(ifp, 1);
		brcmf_configure_arp_nd_offload(ifp, true);
	}
	return err;
}
