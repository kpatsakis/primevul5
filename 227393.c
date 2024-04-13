int __hci_req_remove_ext_adv_instance(struct hci_request *req, u8 instance)
{
	struct hci_dev *hdev = req->hdev;

	/* If request specifies an instance that doesn't exist, fail */
	if (instance > 0 && !hci_find_adv_instance(hdev, instance))
		return -EINVAL;

	hci_req_add(req, HCI_OP_LE_REMOVE_ADV_SET, sizeof(instance), &instance);

	return 0;
}