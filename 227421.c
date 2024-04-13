void hci_req_sync_cancel(struct hci_dev *hdev, int err)
{
	bt_dev_dbg(hdev, "err 0x%2.2x", err);

	if (hdev->req_status == HCI_REQ_PEND) {
		hdev->req_result = err;
		hdev->req_status = HCI_REQ_CANCELED;
		wake_up_interruptible(&hdev->req_wait_q);
	}
}