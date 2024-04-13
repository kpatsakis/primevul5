static int bredr_inquiry(struct hci_request *req, unsigned long opt)
{
	u8 length = opt;
	const u8 giac[3] = { 0x33, 0x8b, 0x9e };
	const u8 liac[3] = { 0x00, 0x8b, 0x9e };
	struct hci_cp_inquiry cp;

	bt_dev_dbg(req->hdev, "");

	hci_dev_lock(req->hdev);
	hci_inquiry_cache_flush(req->hdev);
	hci_dev_unlock(req->hdev);

	memset(&cp, 0, sizeof(cp));

	if (req->hdev->discovery.limited)
		memcpy(&cp.lap, liac, sizeof(cp.lap));
	else
		memcpy(&cp.lap, giac, sizeof(cp.lap));

	cp.length = length;

	hci_req_add(req, HCI_OP_INQUIRY, sizeof(cp), &cp);

	return 0;
}