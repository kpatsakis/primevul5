static void hci_le_conn_update_complete_evt(struct hci_dev *hdev,
					    struct sk_buff *skb)
{
	struct hci_ev_le_conn_update_complete *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	if (ev->status)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->handle));
	if (conn) {
		conn->le_conn_interval = le16_to_cpu(ev->interval);
		conn->le_conn_latency = le16_to_cpu(ev->latency);
		conn->le_supv_timeout = le16_to_cpu(ev->supervision_timeout);
	}

	hci_dev_unlock(hdev);
}