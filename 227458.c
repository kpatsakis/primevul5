static void interleave_scan_work(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    interleave_scan.work);
	u8 status;
	unsigned long timeout;

	if (hdev->interleave_scan_state == INTERLEAVE_SCAN_ALLOWLIST) {
		timeout = msecs_to_jiffies(hdev->advmon_allowlist_duration);
	} else if (hdev->interleave_scan_state == INTERLEAVE_SCAN_NO_FILTER) {
		timeout = msecs_to_jiffies(hdev->advmon_no_filter_duration);
	} else {
		bt_dev_err(hdev, "unexpected error");
		return;
	}

	hci_req_sync(hdev, hci_req_add_le_interleaved_scan, 0,
		     HCI_CMD_TIMEOUT, &status);

	/* Don't continue interleaving if it was canceled */
	if (is_interleave_scanning(hdev))
		queue_delayed_work(hdev->req_workqueue,
				   &hdev->interleave_scan, timeout);
}