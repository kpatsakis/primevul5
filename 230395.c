static void hci_cc_le_set_ext_scan_enable(struct hci_dev *hdev,
				      struct sk_buff *skb)
{
	struct hci_cp_le_set_ext_scan_enable *cp;
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_EXT_SCAN_ENABLE);
	if (!cp)
		return;

	le_set_scan_enable_complete(hdev, cp->enable);
}