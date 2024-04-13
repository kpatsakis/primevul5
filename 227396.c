static int discoverable_update(struct hci_request *req, unsigned long opt)
{
	struct hci_dev *hdev = req->hdev;

	hci_dev_lock(hdev);

	if (hci_dev_test_flag(hdev, HCI_BREDR_ENABLED)) {
		write_iac(req);
		__hci_req_update_scan(req);
		__hci_req_update_class(req);
	}

	/* Advertising instances don't use the global discoverable setting, so
	 * only update AD if advertising was enabled using Set Advertising.
	 */
	if (hci_dev_test_flag(hdev, HCI_ADVERTISING)) {
		__hci_req_update_adv_data(req, 0x00);

		/* Discoverable mode affects the local advertising
		 * address in limited privacy mode.
		 */
		if (hci_dev_test_flag(hdev, HCI_LIMITED_PRIVACY)) {
			if (ext_adv_capable(hdev))
				__hci_req_start_ext_adv(req, 0x00);
			else
				__hci_req_enable_advertising(req);
		}
	}

	hci_dev_unlock(hdev);

	return 0;
}