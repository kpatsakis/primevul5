static void hci_cc_read_tx_power(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_cp_read_tx_power *sent;
	struct hci_rp_read_tx_power *rp = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_READ_TX_POWER);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(rp->handle));
	if (!conn)
		goto unlock;

	switch (sent->type) {
	case 0x00:
		conn->tx_power = rp->tx_power;
		break;
	case 0x01:
		conn->max_tx_power = rp->tx_power;
		break;
	}

unlock:
	hci_dev_unlock(hdev);
}