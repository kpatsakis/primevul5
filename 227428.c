static void start_discovery(struct hci_dev *hdev, u8 *status)
{
	unsigned long timeout;

	bt_dev_dbg(hdev, "type %u", hdev->discovery.type);

	switch (hdev->discovery.type) {
	case DISCOV_TYPE_BREDR:
		if (!hci_dev_test_flag(hdev, HCI_INQUIRY))
			hci_req_sync(hdev, bredr_inquiry,
				     DISCOV_BREDR_INQUIRY_LEN, HCI_CMD_TIMEOUT,
				     status);
		return;
	case DISCOV_TYPE_INTERLEAVED:
		/* When running simultaneous discovery, the LE scanning time
		 * should occupy the whole discovery time sine BR/EDR inquiry
		 * and LE scanning are scheduled by the controller.
		 *
		 * For interleaving discovery in comparison, BR/EDR inquiry
		 * and LE scanning are done sequentially with separate
		 * timeouts.
		 */
		if (test_bit(HCI_QUIRK_SIMULTANEOUS_DISCOVERY,
			     &hdev->quirks)) {
			timeout = msecs_to_jiffies(DISCOV_LE_TIMEOUT);
			/* During simultaneous discovery, we double LE scan
			 * interval. We must leave some time for the controller
			 * to do BR/EDR inquiry.
			 */
			hci_req_sync(hdev, interleaved_discov,
				     hdev->le_scan_int_discovery * 2, HCI_CMD_TIMEOUT,
				     status);
			break;
		}

		timeout = msecs_to_jiffies(hdev->discov_interleaved_timeout);
		hci_req_sync(hdev, active_scan, hdev->le_scan_int_discovery,
			     HCI_CMD_TIMEOUT, status);
		break;
	case DISCOV_TYPE_LE:
		timeout = msecs_to_jiffies(DISCOV_LE_TIMEOUT);
		hci_req_sync(hdev, active_scan, hdev->le_scan_int_discovery,
			     HCI_CMD_TIMEOUT, status);
		break;
	default:
		*status = HCI_ERROR_UNSPECIFIED;
		return;
	}

	if (*status)
		return;

	bt_dev_dbg(hdev, "timeout %u ms", jiffies_to_msecs(timeout));

	/* When service discovery is used and the controller has a
	 * strict duplicate filter, it is important to remember the
	 * start and duration of the scan. This is required for
	 * restarting scanning during the discovery phase.
	 */
	if (test_bit(HCI_QUIRK_STRICT_DUPLICATE_FILTER, &hdev->quirks) &&
		     hdev->discovery.result_filtering) {
		hdev->discovery.scan_start = jiffies;
		hdev->discovery.scan_duration = timeout;
	}

	queue_delayed_work(hdev->req_workqueue, &hdev->le_scan_disable,
			   timeout);
}