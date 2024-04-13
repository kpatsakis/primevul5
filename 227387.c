static int update_bg_scan(struct hci_request *req, unsigned long opt)
{
	hci_dev_lock(req->hdev);
	__hci_update_background_scan(req);
	hci_dev_unlock(req->hdev);
	return 0;
}