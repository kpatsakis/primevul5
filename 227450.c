void __hci_req_disable_advertising(struct hci_request *req)
{
	if (ext_adv_capable(req->hdev)) {
		__hci_req_disable_ext_adv_instance(req, 0x00);

	} else {
		u8 enable = 0x00;

		hci_req_add(req, HCI_OP_LE_SET_ADV_ENABLE, sizeof(enable), &enable);
	}
}