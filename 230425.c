static void hci_cc_le_read_def_data_len(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	struct hci_rp_le_read_def_data_len *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	hdev->le_def_tx_len = le16_to_cpu(rp->tx_len);
	hdev->le_def_tx_time = le16_to_cpu(rp->tx_time);
}