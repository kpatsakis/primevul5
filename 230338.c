static void hci_cc_write_ssp_debug_mode(struct hci_dev *hdev, struct sk_buff *skb)
{
	u8 status = *((u8 *) skb->data);
	u8 *mode;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	mode = hci_sent_cmd_data(hdev, HCI_OP_WRITE_SSP_DEBUG_MODE);
	if (mode)
		hdev->ssp_debug_mode = *mode;
}