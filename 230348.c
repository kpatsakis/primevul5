static void hci_cc_le_set_default_phy(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);
	struct hci_cp_le_set_default_phy *cp;

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status)
		return;

	cp = hci_sent_cmd_data(hdev, HCI_OP_LE_SET_DEFAULT_PHY);
	if (!cp)
		return;

	hci_dev_lock(hdev);

	hdev->le_tx_def_phys = cp->tx_phys;
	hdev->le_rx_def_phys = cp->rx_phys;

	hci_dev_unlock(hdev);
}