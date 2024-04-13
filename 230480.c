static void hci_cc_write_ssp_mode(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	struct hci_cp_write_ssp_mode *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_SSP_MODE);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	if (!status) {
		if (sent->mode)
			hdev->features[1][0] |= LMP_HOST_SSP;
		else
			hdev->features[1][0] &= ~LMP_HOST_SSP;
	}

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		mgmt_ssp_enable_complete(hdev, sent->mode, status);
	else if (!status) {
		if (sent->mode)
			hci_dev_set_flag(hdev, HCI_SSP_ENABLED);
		else
			hci_dev_clear_flag(hdev, HCI_SSP_ENABLED);
	}

	hci_dev_unlock(hdev);
}