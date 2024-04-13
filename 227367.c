static void scan_update_work(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev, scan_update);

	hci_req_sync(hdev, update_scan, 0, HCI_CMD_TIMEOUT, NULL);
}