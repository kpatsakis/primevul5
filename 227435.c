static int update_scan(struct hci_request *req, unsigned long opt)
{
	hci_dev_lock(req->hdev);
	__hci_req_update_scan(req);
	hci_dev_unlock(req->hdev);
	return 0;
}