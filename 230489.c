static void hci_cc_le_write_def_data_len(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_cp_le_write_def_data_len *sent;
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_LE_WRITE_DEF_DATA_LEN);
	if (!sent)
		return;

	hdev->le_def_tx_len = le16_to_cpu(sent->tx_len);
	hdev->le_def_tx_time = le16_to_cpu(sent->tx_time);
}