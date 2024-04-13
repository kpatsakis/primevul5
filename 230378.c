static void le_set_scan_enable_complete(struct hci_dev *hdev, u8 enable)
{
	hci_dev_lock(hdev);

	switch (enable) {
	case LE_SCAN_ENABLE:
		hci_dev_set_flag(hdev, HCI_LE_SCAN);
		if (hdev->le_scan_type == LE_SCAN_ACTIVE)
			clear_pending_adv_report(hdev);
		break;

	case LE_SCAN_DISABLE:
		/* We do this here instead of when setting DISCOVERY_STOPPED
		 * since the latter would potentially require waiting for
		 * inquiry to stop too.
		 */
		if (has_pending_adv_report(hdev)) {
			struct discovery_state *d = &hdev->discovery;

			mgmt_device_found(hdev, &d->last_adv_addr, LE_LINK,
					  d->last_adv_addr_type, NULL,
					  d->last_adv_rssi, d->last_adv_flags,
					  d->last_adv_data,
					  d->last_adv_data_len, NULL, 0);
		}

		/* Cancel this timer so that we don't try to disable scanning
		 * when it's already disabled.
		 */
		cancel_delayed_work(&hdev->le_scan_disable);

		hci_dev_clear_flag(hdev, HCI_LE_SCAN);

		/* The HCI_LE_SCAN_INTERRUPTED flag indicates that we
		 * interrupted scanning due to a connect request. Mark
		 * therefore discovery as stopped. If this was not
		 * because of a connect request advertising might have
		 * been disabled because of active scanning, so
		 * re-enable it again if necessary.
		 */
		if (hci_dev_test_and_clear_flag(hdev, HCI_LE_SCAN_INTERRUPTED))
			hci_discovery_set_state(hdev, DISCOVERY_STOPPED);
		else if (!hci_dev_test_flag(hdev, HCI_LE_ADV) &&
			 hdev->discovery.state == DISCOVERY_FINDING)
			hci_req_reenable_advertising(hdev);

		break;

	default:
		bt_dev_err(hdev, "use of reserved LE_Scan_Enable param %d",
			   enable);
		break;
	}

	hci_dev_unlock(hdev);
}