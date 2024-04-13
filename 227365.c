static void le_scan_restart_work(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    le_scan_restart.work);
	unsigned long timeout, duration, scan_start, now;
	u8 status;

	bt_dev_dbg(hdev, "");

	hci_req_sync(hdev, le_scan_restart, 0, HCI_CMD_TIMEOUT, &status);
	if (status) {
		bt_dev_err(hdev, "failed to restart LE scan: status %d",
			   status);
		return;
	}

	hci_dev_lock(hdev);

	if (!test_bit(HCI_QUIRK_STRICT_DUPLICATE_FILTER, &hdev->quirks) ||
	    !hdev->discovery.scan_start)
		goto unlock;

	/* When the scan was started, hdev->le_scan_disable has been queued
	 * after duration from scan_start. During scan restart this job
	 * has been canceled, and we need to queue it again after proper
	 * timeout, to make sure that scan does not run indefinitely.
	 */
	duration = hdev->discovery.scan_duration;
	scan_start = hdev->discovery.scan_start;
	now = jiffies;
	if (now - scan_start <= duration) {
		int elapsed;

		if (now >= scan_start)
			elapsed = now - scan_start;
		else
			elapsed = ULONG_MAX - scan_start + now;

		timeout = duration - elapsed;
	} else {
		timeout = 0;
	}

	queue_delayed_work(hdev->req_workqueue,
			   &hdev->le_scan_disable, timeout);

unlock:
	hci_dev_unlock(hdev);
}