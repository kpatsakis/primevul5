int hci_req_run(struct hci_request *req, hci_req_complete_t complete)
{
	return req_run(req, complete, NULL);
}