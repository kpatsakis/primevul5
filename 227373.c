static bool scan_use_rpa(struct hci_dev *hdev)
{
	return hci_dev_test_flag(hdev, HCI_PRIVACY);
}