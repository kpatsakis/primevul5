static void discoverable_update_work(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    discoverable_update);
	u8 status;

	hci_req_sync(hdev, discoverable_update, 0, HCI_CMD_TIMEOUT, &status);
	mgmt_set_discoverable_complete(hdev, status);
}