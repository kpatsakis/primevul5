int __hci_req_start_ext_adv(struct hci_request *req, u8 instance)
{
	struct hci_dev *hdev = req->hdev;
	struct adv_info *adv_instance = hci_find_adv_instance(hdev, instance);
	int err;

	/* If instance isn't pending, the chip knows about it, and it's safe to
	 * disable
	 */
	if (adv_instance && !adv_instance->pending)
		__hci_req_disable_ext_adv_instance(req, instance);

	err = __hci_req_setup_ext_adv_instance(req, instance);
	if (err < 0)
		return err;

	__hci_req_update_scan_rsp_data(req, instance);
	__hci_req_enable_ext_advertising(req, instance);

	return 0;
}