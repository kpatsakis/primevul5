static void hci_cc_write_auth_payload_timeout(struct hci_dev *hdev,
					      struct sk_buff *skb)
{
	struct hci_rp_write_auth_payload_to *rp = (void *)skb->data;
	struct hci_conn *conn;
	void *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_AUTH_PAYLOAD_TO);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(rp->handle));
	if (conn)
		conn->auth_payload_timeout = get_unaligned_le16(sent + 2);

	hci_dev_unlock(hdev);
}