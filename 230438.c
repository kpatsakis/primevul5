static void hci_cc_write_page_scan_type(struct hci_dev *hdev,
					struct sk_buff *skb)
{
	u8 status = *((u8 *) skb->data);
	u8 *type;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	type = hci_sent_cmd_data(hdev, HCI_OP_WRITE_PAGE_SCAN_TYPE);
	if (type)
		hdev->page_scan_type = *type;
}