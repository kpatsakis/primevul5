static void hci_cc_write_local_name(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	void *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_LOCAL_NAME);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		mgmt_set_local_name_complete(hdev, sent, status);
	else if (!status)
		memcpy(hdev->dev_name, sent, HCI_MAX_NAME_LENGTH);

	hci_dev_unlock(hdev);
}