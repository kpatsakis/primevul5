static void hci_cs_inquiry(struct hci_dev *hdev, __u8 status)
{
	BT_DBG("%s status 0x%2.2x", hdev->name, status);

	if (status) {
		hci_conn_check_pending(hdev);
		return;
	}

	set_bit(HCI_INQUIRY, &hdev->flags);
}