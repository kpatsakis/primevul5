static void hci_cc_le_del_from_white_list(struct hci_dev *hdev,
					  struct sk_buff *skb)
{
	struct hci_cp_le_del_from_white_list *sent;
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_LE_DEL_FROM_WHITE_LIST);
	if (!sent)
		return;

	hci_bdaddr_list_del(&hdev->le_white_list, &sent->bdaddr,
			    sent->bdaddr_type);
}