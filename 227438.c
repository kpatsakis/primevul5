static u8 update_white_list(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_conn_params *params;
	struct bdaddr_list *b;
	u8 num_entries = 0;
	bool pend_conn, pend_report;
	/* We allow whitelisting even with RPAs in suspend. In the worst case,
	 * we won't be able to wake from devices that use the privacy1.2
	 * features. Additionally, once we support privacy1.2 and IRK
	 * offloading, we can update this to also check for those conditions.
	 */
	bool allow_rpa = hdev->suspended;

	/* Go through the current white list programmed into the
	 * controller one by one and check if that address is still
	 * in the list of pending connections or list of devices to
	 * report. If not present in either list, then queue the
	 * command to remove it from the controller.
	 */
	list_for_each_entry(b, &hdev->le_white_list, list) {
		pend_conn = hci_pend_le_action_lookup(&hdev->pend_le_conns,
						      &b->bdaddr,
						      b->bdaddr_type);
		pend_report = hci_pend_le_action_lookup(&hdev->pend_le_reports,
							&b->bdaddr,
							b->bdaddr_type);

		/* If the device is not likely to connect or report,
		 * remove it from the whitelist.
		 */
		if (!pend_conn && !pend_report) {
			del_from_white_list(req, &b->bdaddr, b->bdaddr_type);
			continue;
		}

		/* White list can not be used with RPAs */
		if (!allow_rpa &&
		    !hci_dev_test_flag(hdev, HCI_ENABLE_LL_PRIVACY) &&
		    hci_find_irk_by_addr(hdev, &b->bdaddr, b->bdaddr_type)) {
			return 0x00;
		}

		num_entries++;
	}

	/* Since all no longer valid white list entries have been
	 * removed, walk through the list of pending connections
	 * and ensure that any new device gets programmed into
	 * the controller.
	 *
	 * If the list of the devices is larger than the list of
	 * available white list entries in the controller, then
	 * just abort and return filer policy value to not use the
	 * white list.
	 */
	list_for_each_entry(params, &hdev->pend_le_conns, action) {
		if (add_to_white_list(req, params, &num_entries, allow_rpa))
			return 0x00;
	}

	/* After adding all new pending connections, walk through
	 * the list of pending reports and also add these to the
	 * white list if there is still space. Abort if space runs out.
	 */
	list_for_each_entry(params, &hdev->pend_le_reports, action) {
		if (add_to_white_list(req, params, &num_entries, allow_rpa))
			return 0x00;
	}

	/* Use the allowlist unless the following conditions are all true:
	 * - We are not currently suspending
	 * - There are 1 or more ADV monitors registered and it's not offloaded
	 * - Interleaved scanning is not currently using the allowlist
	 */
	if (!idr_is_empty(&hdev->adv_monitors_idr) && !hdev->suspended &&
	    hci_get_adv_monitor_offload_ext(hdev) == HCI_ADV_MONITOR_EXT_NONE &&
	    hdev->interleave_scan_state != INTERLEAVE_SCAN_ALLOWLIST)
		return 0x00;

	/* Select filter policy to use white list */
	return 0x01;
}