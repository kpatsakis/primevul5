void hci_req_disable_address_resolution(struct hci_dev *hdev)
{
	struct hci_request req;
	__u8 enable = 0x00;

	if (!use_ll_privacy(hdev) &&
	    !hci_dev_test_flag(hdev, HCI_LL_RPA_RESOLUTION))
		return;

	hci_req_init(&req, hdev);

	hci_req_add(&req, HCI_OP_LE_SET_ADDR_RESOLV_ENABLE, 1, &enable);

	hci_req_run(&req, enable_addr_resolution_complete);
}