static void hci_cc_reset(struct hci_dev *hdev, struct sk_buff *skb)
{
	__u8 status = *((__u8 *) skb->data);

	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	clear_bit(HCI_RESET, &hdev->flags);

	if (status)
		return;

	/* Reset all non-persistent flags */
	hci_dev_clear_volatile_flags(hdev);

	hci_discovery_set_state(hdev, DISCOVERY_STOPPED);

	hdev->inq_tx_power = HCI_TX_POWER_INVALID;
	hdev->adv_tx_power = HCI_TX_POWER_INVALID;

	memset(hdev->adv_data, 0, sizeof(hdev->adv_data));
	hdev->adv_data_len = 0;

	memset(hdev->scan_rsp_data, 0, sizeof(hdev->scan_rsp_data));
	hdev->scan_rsp_data_len = 0;

	hdev->le_scan_type = LE_SCAN_PASSIVE;

	hdev->ssp_debug_mode = 0;

	hci_bdaddr_list_clear(&hdev->le_white_list);
	hci_bdaddr_list_clear(&hdev->le_resolv_list);
}