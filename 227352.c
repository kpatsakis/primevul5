void hci_req_add_le_scan_disable(struct hci_request *req, bool rpa_le_conn)
{
	struct hci_dev *hdev = req->hdev;

	if (hdev->scanning_paused) {
		bt_dev_dbg(hdev, "Scanning is paused for suspend");
		return;
	}

	if (hdev->suspended)
		set_bit(SUSPEND_SCAN_DISABLE, hdev->suspend_tasks);

	if (use_ext_scan(hdev)) {
		struct hci_cp_le_set_ext_scan_enable cp;

		memset(&cp, 0, sizeof(cp));
		cp.enable = LE_SCAN_DISABLE;
		hci_req_add(req, HCI_OP_LE_SET_EXT_SCAN_ENABLE, sizeof(cp),
			    &cp);
	} else {
		struct hci_cp_le_set_scan_enable cp;

		memset(&cp, 0, sizeof(cp));
		cp.enable = LE_SCAN_DISABLE;
		hci_req_add(req, HCI_OP_LE_SET_SCAN_ENABLE, sizeof(cp), &cp);
	}

	/* Disable address resolution */
	if (use_ll_privacy(hdev) &&
	    hci_dev_test_flag(hdev, HCI_ENABLE_LL_PRIVACY) &&
	    hci_dev_test_flag(hdev, HCI_LL_RPA_RESOLUTION) && !rpa_le_conn) {
		__u8 enable = 0x00;

		hci_req_add(req, HCI_OP_LE_SET_ADDR_RESOLV_ENABLE, 1, &enable);
	}
}