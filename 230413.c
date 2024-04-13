static void hci_cc_le_set_ext_scan_param(struct hci_dev *hdev,
					 struct sk_buff *skb)
{
	struct hci_cp_le_set_ext_scan_params *cp;
	__u8 status = *((__u8 *) skb->data);
	struct hci_cp_le_scan_phy_params *phy_param;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_EXT_SCAN_PARAMS);
	if (!cp)
		return;

	phy_param = (void *)cp->data;

	hci_dev_lock(hdev);

	hdev->le_scan_type = phy_param->type;

	hci_dev_unlock(hdev);
}