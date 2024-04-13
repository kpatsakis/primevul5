static void hci_cs_disconnect(struct hci_dev *hdev, u8 status)
{
	struct hci_cp_disconnect *cp;
	struct hci_conn *conn;

	if (!status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_DISCONNECT);
	if (!cp)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(cp->handle));
	if (conn) {
		u8 type = conn->type;

		mgmt_disconnect_failed(hdev, &conn->dst, conn->type,
				       conn->dst_type, status);

		/* If the disconnection failed for any reason, the upper layer
		 * does not retry to disconnect in current implementation.
		 * Hence, we need to do some basic cleanup here and re-enable
		 * advertising if necessary.
		 */
		hci_conn_del(conn);
		if (type == LE_LINK)
			hci_req_reenable_advertising(hdev);
	}

	hci_dev_unlock(hdev);
}