static int le_scan_disable(struct hci_request *req, unsigned long opt)
{
	hci_req_add_le_scan_disable(req, false);
	return 0;
}