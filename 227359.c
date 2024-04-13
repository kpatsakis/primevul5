static bool is_interleave_scanning(struct hci_dev *hdev)
{
	return hdev->interleave_scan_state != INTERLEAVE_SCAN_NONE;
}