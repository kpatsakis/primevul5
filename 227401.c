bool hci_req_status_pend(struct hci_dev *hdev)
{
	return hdev->req_status == HCI_REQ_PEND;
}