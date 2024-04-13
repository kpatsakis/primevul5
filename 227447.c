int hci_req_update_adv_data(struct hci_dev *hdev, u8 instance)
{
	struct hci_request req;

	hci_req_init(&req, hdev);
	__hci_req_update_adv_data(&req, instance);

	return hci_req_run(&req, NULL);
}