static int interleaved_discov(struct hci_request *req, unsigned long opt)
{
	int err;

	bt_dev_dbg(req->hdev, "");

	err = active_scan(req, opt);
	if (err)
		return err;

	return bredr_inquiry(req, DISCOV_BREDR_INQUIRY_LEN);
}