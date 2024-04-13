static void hci_le_phy_update_evt(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_ev_le_phy_update_complete *ev = (void *) skb->data;
	struct hci_conn *conn;

	BT_DBG("%s status 0x%2.2x", hdev->name, ev->status);

	if (!ev->status)
		return;

	hci_dev_lock(hdev);

	conn = hci_conn_hash_lookup_handle(hdev, __le16_to_cpu(ev->handle));
	if (!conn)
		goto unlock;

	conn->le_tx_phy = ev->tx_phy;
	conn->le_rx_phy = ev->rx_phy;

unlock:
	hci_dev_unlock(hdev);
}