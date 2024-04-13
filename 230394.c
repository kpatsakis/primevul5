static void hci_cc_write_class_of_dev(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	void *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_CLASS_OF_DEV);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	if (status == 0)
		memcpy(hdev->dev_class, sent, 3);

	if (hci_dev_test_flag(hdev, HCI_MGMT))
		mgmt_set_class_of_dev_complete(hdev, sent, status);

	hci_dev_unlock(hdev);
}