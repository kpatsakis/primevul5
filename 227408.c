void __hci_req_update_adv_data(struct hci_request *req, u8 instance)
{
	struct hci_dev *hdev = req->hdev;
	u8 len;

	if (!hci_dev_test_flag(hdev, HCI_LE_ENABLED))
		return;

	if (ext_adv_capable(hdev)) {
		struct hci_cp_le_set_ext_adv_data cp;

		memset(&cp, 0, sizeof(cp));

		len = create_instance_adv_data(hdev, instance, cp.data);

		/* There's nothing to do if the data hasn't changed */
		if (hdev->adv_data_len == len &&
		    memcmp(cp.data, hdev->adv_data, len) == 0)
			return;

		memcpy(hdev->adv_data, cp.data, sizeof(cp.data));
		hdev->adv_data_len = len;

		cp.length = len;
		cp.handle = instance;
		cp.operation = LE_SET_ADV_DATA_OP_COMPLETE;
		cp.frag_pref = LE_SET_ADV_DATA_NO_FRAG;

		hci_req_add(req, HCI_OP_LE_SET_EXT_ADV_DATA, sizeof(cp), &cp);
	} else {
		struct hci_cp_le_set_adv_data cp;

		memset(&cp, 0, sizeof(cp));

		len = create_instance_adv_data(hdev, instance, cp.data);

		/* There's nothing to do if the data hasn't changed */
		if (hdev->adv_data_len == len &&
		    memcmp(cp.data, hdev->adv_data, len) == 0)
			return;

		memcpy(hdev->adv_data, cp.data, sizeof(cp.data));
		hdev->adv_data_len = len;

		cp.length = len;

		hci_req_add(req, HCI_OP_LE_SET_ADV_DATA, sizeof(cp), &cp);
	}
}