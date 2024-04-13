static void hci_cc_le_set_ext_adv_enable(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_cp_le_set_ext_adv_enable *cp;
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_EXT_ADV_ENABLE);
	if (!cp)
		return;

	hci_dev_lock(hdev);

	if (cp->enable) {
		struct hci_conn *conn;

		hci_dev_set_flag(hdev, HCI_LE_ADV);

		conn = hci_lookup_le_connect(hdev);
		if (conn)
			queue_delayed_work(hdev->workqueue,
					   &conn->le_conn_timeout,
					   conn->conn_timeout);
	} else {
		hci_dev_clear_flag(hdev, HCI_LE_ADV);
	}

	hci_dev_unlock(hdev);
}