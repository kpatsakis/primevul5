static int le_scan_restart(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;

	/* If controller is not scanning we are done. */
	if (!hci_dev_test_flag(hdev, HCI_LE_SCAN))
		return 0;

	if (hdev->scanning_paused) {
		bt_dev_dbg(hdev, "Scanning is paused for suspend");
		return 0;
	}

	hci_req_add_le_scan_disable(req, false);

	if (use_ext_scan(hdev)) {
		struct hci_cp_le_set_ext_scan_enable ext_enable_cp;

		memset(&ext_enable_cp, 0, sizeof(ext_enable_cp));
		ext_enable_cp.enable = LE_SCAN_ENABLE;
		ext_enable_cp.filter_dup = LE_SCAN_FILTER_DUP_ENABLE;

		hci_req_add(req, HCI_OP_LE_SET_EXT_SCAN_ENABLE,
			    sizeof(ext_enable_cp), &ext_enable_cp);
	} else {
		struct hci_cp_le_set_scan_enable cp;

		memset(&cp, 0, sizeof(cp));
		cp.enable = LE_SCAN_ENABLE;
		cp.filter_dup = LE_SCAN_FILTER_DUP_ENABLE;
		hci_req_add(req, HCI_OP_LE_SET_SCAN_ENABLE, sizeof(cp), &cp);
	}

	return 0;
}