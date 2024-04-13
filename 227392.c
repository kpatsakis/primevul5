static void discov_off(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    discov_off.work);

	bt_dev_dbg(hdev, "");

	hci_dev_lock(hdev);

	/* When discoverable timeout triggers, then just make sure
	 * the limited discoverable flag is cleared. Even in the case
	 * of a timeout triggered from general discoverable, it is
	 * safe to unconditionally clear the flag.
	 */
	hci_dev_clear_flag(hdev, HCI_LIMITED_DISCOVERABLE);
	hci_dev_clear_flag(hdev, HCI_DISCOVERABLE);
	hdev->discov_timeout = 0;

	hci_dev_unlock(hdev);

	hci_req_sync(hdev, discoverable_update, 0, HCI_CMD_TIMEOUT, NULL);
	mgmt_new_settings(hdev);
}