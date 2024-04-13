static int hci_req_add_le_interleaved_scan(struct hci_request *req,
					   unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;
	int ret = 0;

	hci_dev_lock(hdev);

	if (hci_dev_test_flag(hdev, HCI_LE_SCAN))
		hci_req_add_le_scan_disable(req, false);
	hci_req_add_le_passive_scan(req);

	switch (hdev->interleave_scan_state) {
	case INTERLEAVE_SCAN_ALLOWLIST:
		bt_dev_dbg(hdev, "next state: allowlist");
		hdev->interleave_scan_state = INTERLEAVE_SCAN_NO_FILTER;
		break;
	case INTERLEAVE_SCAN_NO_FILTER:
		bt_dev_dbg(hdev, "next state: no filter");
		hdev->interleave_scan_state = INTERLEAVE_SCAN_ALLOWLIST;
		break;
	case INTERLEAVE_SCAN_NONE:
		BT_ERR("unexpected error");
		ret = -1;
	}

	hci_dev_unlock(hdev);

	return ret;
}