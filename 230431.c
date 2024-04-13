static void hci_cs_le_start_enc(struct hci_dev *hdev, u8 status)
{
	struct hci_cp_le_start_enc *cp;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (!status)
		return;

	hci_dev_lock(hdev);

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_START_ENC);
	if (!cp)
		goto unlock;

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(cp->handle));
	if (!conn)
		goto unlock;

	if (conn->state != BT_CONNECTED)
		goto unlock;

	hci_disconnect(conn, HCI_ERROR_AUTH_FAILURE);
	hci_conn_drop(conn);

unlock:
	hci_dev_unlock(hdev);
}