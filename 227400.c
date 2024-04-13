static void hci_req_add_set_adv_filter_enable(struct hci_request *req,
					      bool enable)
{
	struct hci_dev *hdev = req->hdev;

	switch (hci_get_adv_monitor_offload_ext(hdev)) {
	case HCI_ADV_MONITOR_EXT_MSFT:
		msft_req_add_set_filter_enable(req, enable);
		break;
	default:
		return;
	}

	/* No need to block when enabling since it's on resume path */
	if (hdev->suspended && !enable)
		set_bit(SUSPEND_SET_ADV_FILTER, hdev->suspend_tasks);
}