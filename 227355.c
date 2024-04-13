static void cancel_interleave_scan(struct hci_dev *hdev)
{
	bt_dev_dbg(hdev, "cancelling interleave scan");

	cancel_delayed_work_sync(&hdev->interleave_scan);

	hdev->interleave_scan_state = INTERLEAVE_SCAN_NONE;
}