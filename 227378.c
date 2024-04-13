static void __hci_update_background_scan(struct hci_request *req)
{
	struct hci_dev *hdev = req->hdev;

	if (!test_bit(HCI_UP, &hdev->flags) ||
	    test_bit(HCI_INIT, &hdev->flags) ||
	    hci_dev_test_flag(hdev, HCI_SETUP) ||
	    hci_dev_test_flag(hdev, HCI_CONFIG) ||
	    hci_dev_test_flag(hdev, HCI_AUTO_OFF) ||
	    hci_dev_test_flag(hdev, HCI_UNREGISTER))
		return;

	/* No point in doing scanning if LE support hasn't been enabled */
	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		return;

	/* If discovery is active don't interfere with it */
	if (hdev->discovery.state != DISCOVERY_STOPPED)
		return;

	/* Reset RSSI and UUID filters when starting background scanning
	 * since these filters are meant for service discovery only.
	 *
	 * The Start Discovery and Start Service Discovery operations
	 * ensure to set proper values for RSSI threshold and UUID
	 * filter list. So it is safe to just reset them here.
	 */
	hci_discovery_filter_clear(hdev);

	bt_dev_dbg(hdev, "ADV monitoring is %s",
		   hci_is_adv_monitoring(hdev) ? "on" : "off");

	if (list_empty(&hdev->pend_le_conns) &&
	    list_empty(&hdev->pend_le_reports) &&
	    !hci_is_adv_monitoring(hdev)) {
		/* If there is no pending LE connections or devices
		 * to be scanned for or no ADV monitors, we should stop the
		 * background scanning.
		 */

		/* If controller is not scanning we are done. */
		if (!hci_dev_test_flag(hdev, HCI_LE_SCAN))
			return;

		hci_req_add_le_scan_disable(req, false);

		bt_dev_dbg(hdev, "stopping background scanning");
	} else {
		/* If there is at least one pending LE connection, we should
		 * keep the background scan running.
		 */

		/* If controller is connecting, we should not start scanning
		 * since some controllers are not able to scan and connect at
		 * the same time.
		 */
		if (hci_lookup_le_connect(hdev))
			return;

		/* If controller is currently scanning, we stop it to ensure we
		 * don't miss any advertising (due to duplicates filter).
		 */
		if (hci_dev_test_flag(hdev, HCI_LE_SCAN))
			hci_req_add_le_scan_disable(req, false);

		hci_req_add_le_passive_scan(req);
		bt_dev_dbg(hdev, "starting background scanning");
	}
}