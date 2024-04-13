static bool disconnected_whitelist_entries(struct hci_dev *hdev)
{
	struct bdaddr_list *b;

	list_for_each_entry(b, &hdev->whitelist, list) {
		struct hci_conn *conn;

		conn = hci_conn_hash_lookup_ba(hdev, ACL_LINK, &b->bdaddr);
		if (!conn)
			return true;

		if (conn->state != BT_CONNECTED && conn->state != BT_CONFIG)
			return true;
	}

	return false;
}