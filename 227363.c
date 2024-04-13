void hci_req_reenable_advertising(struct hci_dev *hdev)
{
	struct hci_request req;

	if (!hci_dev_test_flag(hdev, HCI_ADVERTISING) &&
	    list_empty(&hdev->adv_instances))
		return;

	hci_req_init(&req, hdev);

	if (hdev->cur_adv_instance) {
		__hci_req_schedule_adv_instance(&req, hdev->cur_adv_instance,
						true);
	} else {
		if (ext_adv_capable(hdev)) {
			__hci_req_start_ext_adv(&req, 0x00);
		} else {
			__hci_req_update_adv_data(&req, 0x00);
			__hci_req_update_scan_rsp_data(&req, 0x00);
			__hci_req_enable_advertising(&req);
		}
	}

	hci_req_run(&req, adv_enable_complete);
}