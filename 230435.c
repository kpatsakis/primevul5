static void hci_change_link_key_complete_evt(struct hci_dev *hdev,
					     struct sk_buff *skb)
{
	struct hci_ev_change_link_key_complete *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->handle));
	if (conn) {
		if (!ev->status)
			set_bit(HCI_CONN_SECURE, &conn->flags);

		clear_bit(HCI_CONN_AUTH_PEND, &conn->flags);

		hci_key_change_cfm(conn, ev->status);
	}

	hci_dev_unlock(hdev);
}