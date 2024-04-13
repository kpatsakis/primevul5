static void hci_user_passkey_notify_evt(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	struct hci_ev_user_passkey_notify *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s", hdev->name);

	conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &ev->bdaddr);
	if (!conn)
		return;

	conn->passkey_notify = __le32_to_cpu(ev->passkey);
	conn->passkey_entered = 0;

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		mgmt_user_passkey_notify(hdev, &conn->dst, conn->type,
					 conn->dst_type, conn->passkey_notify,
					 conn->passkey_entered);
}