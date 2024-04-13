static void hci_cc_write_encrypt_mode(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	__u8 param;
	void *sent;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_WRITE_ENCRYPT_MODE);
	if (!sent)
		return;

	param = *((__u8 *) sent);

	if (param)
		set_bit(HCI_ENCRYPT, &hdev->flags);
	else
		clear_bit(HCI_ENCRYPT, &hdev->flags);
}