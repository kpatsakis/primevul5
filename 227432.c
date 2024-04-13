static void le_scan_disable_work(struct work_struct *work)
{
	struct hci_dev *hdev = container_of(work, struct hci_dev,
					    le_scan_disable.work);
	u8 status;

	bt_dev_dbg(hdev, "");

	if (!hci_dev_test_flag(hdev, HCI_LE_SCAN))
		return;

	cancel_delayed_work(&hdev->le_scan_restart);

	hci_req_sync(hdev, le_scan_disable, 0, HCI_CMD_TIMEOUT, &status);
	if (status) {
		bt_dev_err(hdev, "failed to disable LE scan: status 0x%02x",
			   status);
		return;
	}

	hdev->discovery.scan_start = 0;

	/* If we were running LE only scan, change discovery state. If
	 * we were running both LE and BR/EDR inquiry simultaneously,
	 * and BR/EDR inquiry is already finished, stop discovery,
	 * otherwise BR/EDR inquiry will stop discovery when finished.
	 * If we will resolve remote device name, do not change
	 * discovery state.
	 */

	if (hdev->discovery.type == DISCOV_TYPE_LE)
		goto discov_stopped;

	if (hdev->discovery.type != DISCOV_TYPE_INTERLEAVED)
		return;

	if (test_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY, &hdev->quirks)) {
		if (!test_bit(HCI_INQUIRY, &hdev->flags) &&
		    hdev->discovery.state != DISCOVERY_RESOLVING)
			goto discov_stopped;

		return;
	}

	hci_req_sync(hdev, bredr_inquiry, DISCOV_INTERLEAVED_INQUIRY_LEN,
		     HCI_CMD_TIMEOUT, &status);
	if (status) {
		bt_dev_err(hdev, "inquiry failed: status 0x%02x", status);
		goto discov_stopped;
	}

	return;

discov_stopped:
	hci_dev_lock(hdev);
	hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
	hci_dev_unlock(hdev);
}