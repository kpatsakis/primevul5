static int active_scan(struct hci_request *req, unsigned long opt)
{
	uint16_t interval = opt;
	struct hci_dev *hdev = req->hdev;
	u8 own_addr_type;
	/* White list is not used for discovery */
	u8 filter_policy = 0x00;
	/* Discovery doesn't require controller address resolution */
	bool addr_resolv = false;
	int err;

	bt_dev_dbg(hdev, "");

	/* If controller is scanning, it means the background scanning is
	 * running. Thus, we should temporarily stop it in order to set the
	 * discovery scanning parameters.
	 */
	if (hci_dev_test_flag(hdev, HCI_LE_SCAN)) {
		hci_req_add_le_scan_disable(req, false);
		cancel_interleave_scan(hdev);
	}

	/* All active scans will be done with either a resolvable private
	 * address (when privacy feature has been enabled) or non-resolvable
	 * private address.
	 */
	err = hci_update_random_address(req, true, scan_use_rpa(hdev),
					&own_addr_type);
	if (err < 0)
		own_addr_type = ADDR_LE_DEV_PUBLIC;

	hci_req_start_scan(req, LE_SCAN_ACTIVE, interval,
			   hdev->le_scan_window_discovery, own_addr_type,
			   filter_policy, addr_resolv);
	return 0;
}