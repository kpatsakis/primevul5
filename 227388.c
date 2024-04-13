static void bg_scan_update(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    bg_scan_update);
	struct hci_conn *conn;
	u8 status;
	int err;

	err = hci_req_sync(hdev, update_bg_scan, 0, HCI_CMD_TIMEOUT, &status);
	if (!err)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_state(hdev, LE_LINK, BT_CONNECT);
	if (conn)
		hci_le_conn_failed(conn, status);

	hci_dev_unlock(hdev);
}