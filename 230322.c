static void hci_pin_code_request_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_pin_code_req *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s", hdev->name);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &ev->bdaddr);
	if (!conn)
		goto unlock;

	if (conn->state == BT_CONNECTED) {
		hci_conn_hold(conn);
		conn->disc_timeout = HCI_PAIRING_TIMEOUT;
		hci_conn_drop(conn);
	}

	if (!hci_dev_test_flag(hdev, HCI_BONDABLE) &&
	    !test_bit(HCI_CONN_AUTH_INITIATOR, &conn->flags)) {
		hci_send_cmd(hdev, HCI_OP_PIN_CODE_NEG_REPLY,
			     sizeof(ev->bdaddr), &ev->bdaddr);
	} else if (hci_dev_test_flag(hdev, HCI_MGMT)) {
		u8 secure;

		if (conn->pending_sec_level == BT_SECURITY_HIGH)
			secure = 1;
		else
			secure = 0;

		mgmt_pin_code_request(hdev, &ev->bdaddr, secure);
	}

unlock:
	hci_dev_unlock(hdev);
}