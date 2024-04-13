static void hci_cc_write_le_host_supported(struct hci_dev *hdev,
					   struct sk_buff *skb)
{
	struct hci_cp_write_le_host_supported *sent;
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_LE_HOST_SUPPORTED);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	if (sent->le) {
		hdev->features[1][0] |= LMP_HOST_LE;
		hci_dev_set_flag(hdev, HCI_LE_ENABLED);
	} else {
		hdev->features[1][0] &= ~LMP_HOST_LE;
		hci_dev_clear_flag(hdev, HCI_LE_ENABLED);
		hci_dev_clear_flag(hdev, HCI_ADVERTISING);
	}

	if (sent->simul)
		hdev->features[1][0] |= LMP_HOST_LE_BREDR;
	else
		hdev->features[1][0] &= ~LMP_HOST_LE_BREDR;

	hci_dev_unlock(hdev);
}