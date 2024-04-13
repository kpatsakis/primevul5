static bool __hci_update_interleaved_scan(struct hci_dev *hdev)
{
	/* Do interleaved scan only if all of the following are true:
	 * - There is at least one ADV monitor
	 * - At least one pending LE connection or one device to be scanned for
	 * - Monitor offloading is not supported
	 * If so, we should alternate between allowlist scan and one without
	 * any filters to save power.
	 */
	bool use_interleaving = hci_is_adv_monitoring(hdev) &&
				!(list_empty(&hdev->pend_le_conns) &&
				  list_empty(&hdev->pend_le_reports)) &&
				hci_get_adv_monitor_offload_ext(hdev) ==
				    HCI_ADV_MONITOR_EXT_NONE;
	bool is_interleaving = is_interleave_scanning(hdev);

	if (use_interleaving && !is_interleaving) {
		start_interleave_scan(hdev);
		bt_dev_dbg(hdev, "starting interleave scan");
		return true;
	}

	if (!use_interleaving && is_interleaving)
		cancel_interleave_scan(hdev);

	return false;
}