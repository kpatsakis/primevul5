int __hci_req_disable_ext_adv_instance(struct hci_request *req, u8 instance)
{
	struct hci_dev *hdev = req->hdev;
	struct hci_cp_le_set_ext_adv_enable *cp;
	struct hci_cp_ext_adv_set *adv_set;
	u8 data[sizeof(*cp) + sizeof(*adv_set) * 1];
	u8 req_size;

	/* If request specifies an instance that doesn't exist, fail */
	if (instance > 0 && !hci_find_adv_instance(hdev, instance))
		return -EINVAL;

	memset(data, 0, sizeof(data));

	cp = (void *)data;
	adv_set = (void *)cp->data;

	/* Instance 0x00 indicates all advertising instances will be disabled */
	cp->num_of_sets = !!instance;
	cp->enable = 0x00;

	adv_set->handle = instance;

	req_size = sizeof(*cp) + sizeof(*adv_set) * cp->num_of_sets;
	hci_req_add(req, HCI_OP_LE_SET_EXT_ADV_ENABLE, req_size, data);

	return 0;
}