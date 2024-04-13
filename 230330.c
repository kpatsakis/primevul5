static void hci_cc_le_read_max_data_len(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	struct hci_rp_le_read_max_data_len *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->le_max_tx_len = le16_to_cpu(rp->tx_len);
	hdev->le_max_tx_time = le16_to_cpu(rp->tx_time);
	hdev->le_max_rx_len = le16_to_cpu(rp->rx_len);
	hdev->le_max_rx_time = le16_to_cpu(rp->rx_time);
}