int hci_req_resume_adv_instances(struct hci_dev *hdev)
{
	struct hci_request req;

	hci_req_init(&req, hdev);
	__hci_req_resume_adv_instances(&req);

	return hci_req_run(&req, NULL);
}