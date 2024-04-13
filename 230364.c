void hci_event_packet(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_event_hdr *hdr = (void *) skb->data;
	hci_req_complete_t req_complete = NULL;
	hci_req_complete_skb_t req_complete_skb = NULL;
	struct sk_buff *orig_skb = NULL;
	u8 status = 0, event = hdr->evt, req_evt = 0;
	u16 opcode = HCI_OP_NOP;

	if (!event) {
		bt_dev_warn(hdev, "Received unexpected HCI Event 00000000");
		goto done;
	}

	if (hdev->sent_cmd && bt_cb(hdev->sent_cmd)->hci.req_event == event) {
		struct hci_command_hdr *cmd_hdr = (void *) hdev->sent_cmd->data;
		opcode = __le16_to_cpu(cmd_hdr->opcode);
		hci_req_cmd_complete(hdev, opcode, status, &req_complete,
				     &req_complete_skb);
		req_evt = event;
	}

	/* If it looks like we might end up having to call
	 * req_complete_skb, store a pristine copy of the skb since the
	 * various handlers may modify the original one through
	 * skb_pull() calls, etc.
	 */
	if (req_complete_skb || event == HCI_EV_CMD_STATUS ||
	    event == HCI_EV_CMD_COMPLETE)
		orig_skb = skb_clone(skb, GFP_KERNEL);

	skb_pull(skb, HCI_EVENT_HDR_SIZE);

	switch (event) {
	case HCI_EV_INQUIRY_COMPLETE:
		hci_inquiry_complete_evt(hdev, skb);
		break;

	case HCI_EV_INQUIRY_RESULT:
		hci_inquiry_result_evt(hdev, skb);
		break;

	case HCI_EV_CONN_COMPLETE:
		hci_conn_complete_evt(hdev, skb);
		break;

	case HCI_EV_CONN_REQUEST:
		hci_conn_request_evt(hdev, skb);
		break;

	case HCI_EV_DISCONN_COMPLETE:
		hci_disconn_complete_evt(hdev, skb);
		break;

	case HCI_EV_AUTH_COMPLETE:
		hci_auth_complete_evt(hdev, skb);
		break;

	case HCI_EV_REMOTE_NAME:
		hci_remote_name_evt(hdev, skb);
		break;

	case HCI_EV_ENCRYPT_CHANGE:
		hci_encrypt_change_evt(hdev, skb);
		break;

	case HCI_EV_CHANGE_LINK_KEY_COMPLETE:
		hci_change_link_key_complete_evt(hdev, skb);
		break;

	case HCI_EV_REMOTE_FEATURES:
		hci_remote_features_evt(hdev, skb);
		break;

	case HCI_EV_CMD_COMPLETE:
		hci_cmd_complete_evt(hdev, skb, &opcode, &status,
				     &req_complete, &req_complete_skb);
		break;

	case HCI_EV_CMD_STATUS:
		hci_cmd_status_evt(hdev, skb, &opcode, &status, &req_complete,
				   &req_complete_skb);
		break;

	case HCI_EV_HARDWARE_ERROR:
		hci_hardware_error_evt(hdev, skb);
		break;

	case HCI_EV_ROLE_CHANGE:
		hci_role_change_evt(hdev, skb);
		break;

	case HCI_EV_NUM_COMP_PKTS:
		hci_num_comp_pkts_evt(hdev, skb);
		break;

	case HCI_EV_MODE_CHANGE:
		hci_mode_change_evt(hdev, skb);
		break;

	case HCI_EV_PIN_CODE_REQ:
		hci_pin_code_request_evt(hdev, skb);
		break;

	case HCI_EV_LINK_KEY_REQ:
		hci_link_key_request_evt(hdev, skb);
		break;

	case HCI_EV_LINK_KEY_NOTIFY:
		hci_link_key_notify_evt(hdev, skb);
		break;

	case HCI_EV_CLOCK_OFFSET:
		hci_clock_offset_evt(hdev, skb);
		break;

	case HCI_EV_PKT_TYPE_CHANGE:
		hci_pkt_type_change_evt(hdev, skb);
		break;

	case HCI_EV_PSCAN_REP_MODE:
		hci_pscan_rep_mode_evt(hdev, skb);
		break;

	case HCI_EV_INQUIRY_RESULT_WITH_RSSI:
		hci_inquiry_result_with_rssi_evt(hdev, skb);
		break;

	case HCI_EV_REMOTE_EXT_FEATURES:
		hci_remote_ext_features_evt(hdev, skb);
		break;

	case HCI_EV_SYNC_CONN_COMPLETE:
		hci_sync_conn_complete_evt(hdev, skb);
		break;

	case HCI_EV_EXTENDED_INQUIRY_RESULT:
		hci_extended_inquiry_result_evt(hdev, skb);
		break;

	case HCI_EV_KEY_REFRESH_COMPLETE:
		hci_key_refresh_complete_evt(hdev, skb);
		break;

	case HCI_EV_IO_CAPA_REQUEST:
		hci_io_capa_request_evt(hdev, skb);
		break;

	case HCI_EV_IO_CAPA_REPLY:
		hci_io_capa_reply_evt(hdev, skb);
		break;

	case HCI_EV_USER_CONFIRM_REQUEST:
		hci_user_confirm_request_evt(hdev, skb);
		break;

	case HCI_EV_USER_PASSKEY_REQUEST:
		hci_user_passkey_request_evt(hdev, skb);
		break;

	case HCI_EV_USER_PASSKEY_NOTIFY:
		hci_user_passkey_notify_evt(hdev, skb);
		break;

	case HCI_EV_KEYPRESS_NOTIFY:
		hci_keypress_notify_evt(hdev, skb);
		break;

	case HCI_EV_SIMPLE_PAIR_COMPLETE:
		hci_simple_pair_complete_evt(hdev, skb);
		break;

	case HCI_EV_REMOTE_HOST_FEATURES:
		hci_remote_host_features_evt(hdev, skb);
		break;

	case HCI_EV_LE_META:
		hci_le_meta_evt(hdev, skb);
		break;

	case HCI_EV_REMOTE_OOB_DATA_REQUEST:
		hci_remote_oob_data_request_evt(hdev, skb);
		break;

#if IS_ENABLED(CONFIG_BT_HS)
	case HCI_EV_CHANNEL_SELECTED:
		hci_chan_selected_evt(hdev, skb);
		break;

	case HCI_EV_PHY_LINK_COMPLETE:
		hci_phy_link_complete_evt(hdev, skb);
		break;

	case HCI_EV_LOGICAL_LINK_COMPLETE:
		hci_loglink_complete_evt(hdev, skb);
		break;

	case HCI_EV_DISCONN_LOGICAL_LINK_COMPLETE:
		hci_disconn_loglink_complete_evt(hdev, skb);
		break;

	case HCI_EV_DISCONN_PHY_LINK_COMPLETE:
		hci_disconn_phylink_complete_evt(hdev, skb);
		break;
#endif

	case HCI_EV_NUM_COMP_BLOCKS:
		hci_num_comp_blocks_evt(hdev, skb);
		break;

	case HCI_EV_VENDOR:
		msft_vendor_evt(hdev, skb);
		break;

	default:
		BT_DBG("%s event 0x%2.2x", hdev->name, event);
		break;
	}

	if (req_complete) {
		req_complete(hdev, status, opcode);
	} else if (req_complete_skb) {
		if (!hci_get_cmd_complete(hdev, opcode, req_evt, orig_skb)) {
			kfree_skb(orig_skb);
			orig_skb = NULL;
		}
		req_complete_skb(hdev, status, opcode, orig_skb);
	}

done:
	kfree_skb(orig_skb);
	kfree_skb(skb);
	hdev->stat.evt_rx++;
}