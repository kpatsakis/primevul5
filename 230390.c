static void hci_cmd_status_evt(struct hci_dev *hdev, struct sk_buff *skb,
			       u16 *opcode, u8 *status,
			       hci_req_complete_t *req_complete,
			       hci_req_complete_skb_t *req_complete_skb)
{
	struct hci_ev_cmd_status *ev = (void *) skb->data;

	skb_pull(skb, sizeof(*ev));

	*opcode = __le16_to_cpu(ev->opcode);
	*status = ev->status;

	switch (*opcode) {
	case HCI_OP_INQUIRY:
		hci_cs_inquiry(hdev, ev->status);
		break;

	case HCI_OP_CREATE_CONN:
		hci_cs_create_conn(hdev, ev->status);
		break;

	case HCI_OP_DISCONNECT:
		hci_cs_disconnect(hdev, ev->status);
		break;

	case HCI_OP_ADD_SCO:
		hci_cs_add_sco(hdev, ev->status);
		break;

	case HCI_OP_AUTH_REQUESTED:
		hci_cs_auth_requested(hdev, ev->status);
		break;

	case HCI_OP_SET_CONN_ENCRYPT:
		hci_cs_set_conn_encrypt(hdev, ev->status);
		break;

	case HCI_OP_REMOTE_NAME_REQ:
		hci_cs_remote_name_req(hdev, ev->status);
		break;

	case HCI_OP_READ_REMOTE_FEATURES:
		hci_cs_read_remote_features(hdev, ev->status);
		break;

	case HCI_OP_READ_REMOTE_EXT_FEATURES:
		hci_cs_read_remote_ext_features(hdev, ev->status);
		break;

	case HCI_OP_SETUP_SYNC_CONN:
		hci_cs_setup_sync_conn(hdev, ev->status);
		break;

	case HCI_OP_SNIFF_MODE:
		hci_cs_sniff_mode(hdev, ev->status);
		break;

	case HCI_OP_EXIT_SNIFF_MODE:
		hci_cs_exit_sniff_mode(hdev, ev->status);
		break;

	case HCI_OP_SWITCH_ROLE:
		hci_cs_switch_role(hdev, ev->status);
		break;

	case HCI_OP_LE_CREATE_CONN:
		hci_cs_le_create_conn(hdev, ev->status);
		break;

	case HCI_OP_LE_READ_REMOTE_FEATURES:
		hci_cs_le_read_remote_features(hdev, ev->status);
		break;

	case HCI_OP_LE_START_ENC:
		hci_cs_le_start_enc(hdev, ev->status);
		break;

	case HCI_OP_LE_EXT_CREATE_CONN:
		hci_cs_le_ext_create_conn(hdev, ev->status);
		break;

	default:
		BT_DBG("%s opcode 0x%4.4x", hdev->name, *opcode);
		break;
	}

	if (*opcode != HCI_OP_NOP)
		cancel_delayed_work(&hdev->cmd_timer);

	if (ev->ncmd && !test_bit(HCI_RESET, &hdev->flags))
		atomic_set(&hdev->cmd_cnt, 1);

	/* Indicate request completion if the command failed. Also, if
	 * we're not waiting for a special event and we get a success
	 * command status we should try to flag the request as completed
	 * (since for this kind of commands there will not be a command
	 * complete event).
	 */
	if (ev->status ||
	    (hdev->sent_cmd && !bt_cb(hdev->sent_cmd)->hci.req_event))
		hci_req_cmd_complete(hdev, *opcode, ev->status, req_complete,
				     req_complete_skb);

	if (hci_dev_test_flag(hdev, HCI_CMD_PENDING)) {
		bt_dev_err(hdev,
			   "unexpected event for opcode 0x%4.4x", *opcode);
		return;
	}

	if (atomic_read(&hdev->cmd_cnt) && !skb_queue_empty(&hdev->cmd_q))
		queue_work(hdev->workqueue, &hdev->cmd_work);
}