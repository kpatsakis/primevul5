static void hci_cc_read_page_scan_activity(struct hci_dev *hdev,
					   struct sk_buff *skb)
{
	struct hci_rp_read_page_scan_activity *rp = (void *) skb->data;

	BT_DBG("%s status 0x%2.2x", hdev->name, rp->status);

	if (rp->status)
		return;

	if (test_bit(HCI_INIT, &hdev->flags)) {
		hdev->page_scan_interval = __le16_to_cpu(rp->interval);
		hdev->page_scan_window = __le16_to_cpu(rp->window);
	}
}