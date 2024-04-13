static void hci_remote_oob_data_request_evt(struct hci_dev *hdev,
					    struct sk_buff *skb)
{
	struct hci_ev_remote_oob_data_request *ev = (void *) skb->data;
	struct oob_data *data;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	if (!hci_dev_test_flag(hdev, HCI_MGMT))
		goto unlock;

	data = hci_find_remote_oob_data(hdev, &ev->bdaddr, BDADDR_BREDR);
	if (!data) {
		struct hci_cp_remote_oob_data_neg_reply cp;

		bacpy(&cp.bdaddr, &ev->bdaddr);
		hci_send_cmd(hdev, HCI_OP_REMOTE_OOB_DATA_NEG_REPLY,
			     sizeof(cp), &cp);
		goto unlock;
	}

	if (bredr_sc_enabled(hdev)) {
		struct hci_cp_remote_oob_ext_data_reply cp;

		bacpy(&cp.bdaddr, &ev->bdaddr);
		if (hci_dev_test_flag(hdev, HCI_SC_ONLY)) {
			memset(cp.hash192, 0, sizeof(cp.hash192));
			memset(cp.rand192, 0, sizeof(cp.rand192));
		} else {
			memcpy(cp.hash192, data->hash192, sizeof(cp.hash192));
			memcpy(cp.rand192, data->rand192, sizeof(cp.rand192));
		}
		memcpy(cp.hash256, data->hash256, sizeof(cp.hash256));
		memcpy(cp.rand256, data->rand256, sizeof(cp.rand256));

		hci_send_cmd(hdev, HCI_OP_REMOTE_OOB_EXT_DATA_REPLY,
			     sizeof(cp), &cp);
	} else {
		struct hci_cp_remote_oob_data_reply cp;

		bacpy(&cp.bdaddr, &ev->bdaddr);
		memcpy(cp.hash, data->hash192, sizeof(cp.hash));
		memcpy(cp.rand, data->rand192, sizeof(cp.rand));

		hci_send_cmd(hdev, HCI_OP_REMOTE_OOB_DATA_REPLY,
			     sizeof(cp), &cp);
	}

unlock:
	hci_dev_unlock(hdev);
}