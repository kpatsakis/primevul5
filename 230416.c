static void hci_cc_set_adv_param(struct hci_dev *hdev, struct sk_buff *skb)
{
	struct hci_cp_le_set_adv_param *cp;
	u8 status = *((u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_ADV_PARAM);
	if (!cp)
		return;

	hci_dev_lock(hdev);
	hdev->adv_addr_type = cp->own_address_type;
	hci_dev_unlock(hdev);
}