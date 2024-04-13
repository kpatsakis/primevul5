static void hci_cc_le_clear_white_list(struct hci_dev *hdev,
				       struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	hci_bdaddr_list_clear(&hdev->le_white_list);
}