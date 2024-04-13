static void hci_cc_write_page_scan_activity(struct hci_dev *hdev,
					    struct sk_buff *skb)
{
	u8 status = *((u8 *) skb->data);
	struct hci_cp_write_page_scan_activity *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_PAGE_SCAN_ACTIVITY);
	if (!sent)
		return;

	hdev->page_scan_interval = __le16_to_cpu(sent->interval);
	hdev->page_scan_window = __le16_to_cpu(sent->window);
}