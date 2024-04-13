static void connectable_update_work(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    connectable_update);
	u8 status;

	hci_req_sync(hdev, connectable_update, 0, HCI_CMD_TIMEOUT, &status);
	mgmt_set_connectable_complete(hdev, status);
}