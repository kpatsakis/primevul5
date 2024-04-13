static void hci_req_clear_event_filter(struct hci_request *req)
{
	struct hci_cp_set_event_filter f;

	memset(&f, 0, sizeof(f));
	f.flt_type = HCI_FLT_CLEAR_ALL;
	hci_req_add(req, HCI_OP_SET_EVENT_FLT, 1, &f);

	/* Update page scan state (since we may have modified it when setting
	 * the event filter).
	 */
	__hci_req_update_scan(req);
}