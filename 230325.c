static void hci_cc_read_auth_payload_timeout(struct hci_dev *hdev,
					     struct sk_buff *skb)
{
	struct hci_rp_read_auth_payload_to *rp = (void *)skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(rp->handle));
	if (conn)
		conn->auth_payload_timeout = __le16_to_cpu(rp->timeout);

	hci_dev_unlock(hdev);
}