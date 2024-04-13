static int stop_discovery(struct hci_request *req, unsigned long opt)
{
	hci_dev_lock(req->hdev);
	hci_req_stop_discovery(req);
	hci_dev_unlock(req->hdev);

	return 0;
}