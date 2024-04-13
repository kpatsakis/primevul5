int __hci_req_sync(struct hci_dev *hdev, int (*func)(struct hci_request *req,
						     unsigned long opt),
		   unsigned long opt, u32 timeout, u8 *hci_status)
{
	struct hci_request req;
	int err = 0;

	bt_dev_dbg(hdev, "start");

	hci_req_init(&req, hdev);

	hdev->req_status = HCI_REQ_PEND;

	err = func(&req, opt);
	if (err) {
		if (hci_status)
			*hci_status = HCI_ERROR_UNSPECIFIED;
		return err;
	}

	err = hci_req_run_skb(&req, hci_req_sync_complete);
	if (err < 0) {
		hdev->req_status = 0;

		/* ENODATA means the HCI request command queue is empty.
		 * This can happen when a request with conditionals doesn't
		 * trigger any commands to be sent. This is normal behavior
		 * and should not trigger an error return.
		 */
		if (err == -ENODATA) {
			if (hci_status)
				*hci_status = 0;
			return 0;
		}

		if (hci_status)
			*hci_status = HCI_ERROR_UNSPECIFIED;

		return err;
	}

	err = wait_event_interruptible_timeout(hdev->req_wait_q,
			hdev->req_status != HCI_REQ_PEND, timeout);

	if (err == -ERESTARTSYS)
		return -EINTR;

	switch (hdev->req_status) {
	case HCI_REQ_DONE:
		err = -bt_to_errno(hdev->req_result);
		if (hci_status)
			*hci_status = hdev->req_result;
		break;

	case HCI_REQ_CANCELED:
		err = -hdev->req_result;
		if (hci_status)
			*hci_status = HCI_ERROR_UNSPECIFIED;
		break;

	default:
		err = -ETIMEDOUT;
		if (hci_status)
			*hci_status = HCI_ERROR_UNSPECIFIED;
		break;
	}

	kfree_skb(hdev->req_skb);
	hdev->req_skb = NULL;
	hdev->req_status = hdev->req_result = 0;

	bt_dev_dbg(hdev, "end: err %d", err);

	return err;
}