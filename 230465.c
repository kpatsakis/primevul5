static void hci_cc_le_set_random_addr(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	bdaddr_t *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_RANDOM_ADDR);
	if (!sent)
		return;

	hci_dev_lock(hdev);

	bacpy(&hdev->random_addr, sent);

	hci_dev_unlock(hdev);
}