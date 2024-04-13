static void hci_req_set_event_filter(struct hci_request *req)
{
	struct bdaddr_list_with_flags *b;
	struct hci_cp_set_event_filter f;
	struct hci_dev *hdev = req->hdev;
	u8 scan = SCAN_DISABLED;

	/* Always clear event filter when starting */
	hci_req_clear_event_filter(req);

	list_for_each_entry(b, &hdev->whitelist, list) {
		if (!hci_conn_test_flag(HCI_CONN_FLAG_REMOTE_WAKEUP,
					b->current_flags))
			continue;

		memset(&f, 0, sizeof(f));
		bacpy(&f.addr_conn_flt.bdaddr, &b->bdaddr);
		f.flt_type = HCI_FLT_CONN_SETUP;
		f.cond_type = HCI_CONN_SETUP_ALLOW_BDADDR;
		f.addr_conn_flt.auto_accept = HCI_CONN_SETUP_AUTO_ON;

		bt_dev_dbg(hdev, "Adding event filters for %pMR", &b->bdaddr);
		hci_req_add(req, HCI_OP_SET_EVENT_FLT, sizeof(f), &f);
		scan = SCAN_PAGE;
	}

	if (scan)
		set_bit(SUSPEND_SCAN_ENABLE, hdev->suspend_tasks);
	else
		set_bit(SUSPEND_SCAN_DISABLE, hdev->suspend_tasks);

	hci_req_add(req, HCI_OP_WRITE_SCAN_ENABLE, 1, &scan);
}