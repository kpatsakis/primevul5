void __hci_req_update_scan_rsp_data(struct hci_request *req, u8 instance)
{
	struct hci_dev *hdev = req->hdev;
	u8 len;

	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		return;

	if (ext_adv_capable(hdev)) {
		struct hci_cp_le_set_ext_scan_rsp_data cp;

		memset(&cp, 0, sizeof(cp));

		if (instance)
			len = create_instance_scan_rsp_data(hdev, instance,
							    cp.data);
		else
			len = create_default_scan_rsp_data(hdev, cp.data);

		if (hdev->scan_rsp_data_len == len &&
		    !memcmp(cp.data, hdev->scan_rsp_data, len))
			return;

		memcpy(hdev->scan_rsp_data, cp.data, sizeof(cp.data));
		hdev->scan_rsp_data_len = len;

		cp.handle = instance;
		cp.length = len;
		cp.operation = LE_SET_ADV_DATA_OP_COMPLETE;
		cp.frag_pref = LE_SET_ADV_DATA_NO_FRAG;

		hci_req_add(req, HCI_OP_LE_SET_EXT_SCAN_RSP_DATA, sizeof(cp),
			    &cp);
	} else {
		struct hci_cp_le_set_scan_rsp_data cp;

		memset(&cp, 0, sizeof(cp));

		if (instance)
			len = create_instance_scan_rsp_data(hdev, instance,
							    cp.data);
		else
			len = create_default_scan_rsp_data(hdev, cp.data);

		if (hdev->scan_rsp_data_len == len &&
		    !memcmp(cp.data, hdev->scan_rsp_data, len))
			return;

		memcpy(hdev->scan_rsp_data, cp.data, sizeof(cp.data));
		hdev->scan_rsp_data_len = len;

		cp.length = len;

		hci_req_add(req, HCI_OP_LE_SET_SCAN_RSP_DATA, sizeof(cp), &cp);
	}
}