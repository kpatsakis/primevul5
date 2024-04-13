static void hci_cc_write_scan_enable(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	__u8 param;
	void *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_SCAN_ENABLE);
	if (!sent)
		return;

	param = *((__u8 *) sent);

	hci_dev_lock(hdev);

	if (status) {
		hdev->discov_timeout = 0;
		goto done;
	}

	if (param & SCAN_INQUIRY)
		set_bit(HCI_ISCAN, &hdev->flags);
	else
		clear_bit(HCI_ISCAN, &hdev->flags);

	if (param & SCAN_PAGE)
		set_bit(HCI_PSCAN, &hdev->flags);
	else
		clear_bit(HCI_PSCAN, &hdev->flags);

done:
	hci_dev_unlock(hdev);
}