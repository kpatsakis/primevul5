static void hci_cc_le_add_to_resolv_list(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_cp_le_add_to_resolv_list *sent;
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	sent = hci_sent_cmd_data(hdev, HCI_OP_LE_ADD_TO_RESOLV_LIST);
	if (!sent)
		return;

	hci_bdaddr_list_add_with_irk(&hdev->le_resolv_list, &sent->bdaddr,
				sent->bdaddr_type, sent->peer_irk,
				sent->local_irk);
}