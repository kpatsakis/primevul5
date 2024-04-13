struct sk_buff *__hci_cmd_sync_ev(struct hci_dev *hdev, u16 opcode, u32 plen,
				  const void *param, u8 event, u32 timeout)
{
	struct hci_request req;
	struct sk_buff *skb;
	int err = 0;

	bt_dev_dbg(hdev, "");

	hci_req_init(&req, hdev);

	hci_req_add_ev(&req, opcode, plen, param, event);

	hdev->req_status = HCI_REQ_PEND;

	err = hci_req_run_skb(&req, hci_req_sync_complete);
	if (err < 0)
		return ERR_PTR(err);

	err = wait_event_interruptible_timeout(hdev->req_wait_q,
			hdev->req_status != HCI_REQ_PEND, timeout);

	if (err == -ERESTARTSYS)
		return ERR_PTR(-EINTR);

	switch (hdev->req_status) {
	case HCI_REQ_DONE:
		err = -bt_to_errno(hdev->req_result);
		break;

	case HCI_REQ_CANCELED:
		err = -hdev->req_result;
		break;

	default:
		err = -ETIMEDOUT;
		break;
	}

	hdev->req_status = hdev->req_result = 0;
	skb = hdev->req_skb;
	hdev->req_skb = NULL;

	bt_dev_dbg(hdev, "end: err %d", err);

	if (err < 0) {
		kfree_skb(skb);
		return ERR_PTR(err);
	}

	if (!skb)
		return ERR_PTR(-ENODATA);

	return skb;
}